//
// Copyright (c) 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
// implied.
// See the License for the specific language governing permissions and
// limitations under the License.


// This source is meant to be compiled as a shared library (.so) on Linux
// and used with the LD_PRELOAD mechanism as a mechanism to intercept
// C library calls. The primary use would be to understand lower level
// details as part of a process or application.
//
// Although many C library functions are intercepted, the ideas is
// that this library is just a very simple pass-through where only
// metrics are captured. Because this code is in the data path, the
// code has to have small footprint and be very efficient. As part of
// this objective, the metrics captured are all passed into the
// "record" function and then passed over-the-wall to some other
// process that will store or analyze the metrics.

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <utime.h>
#include <time.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/types.h>
#ifndef __FreeBSD__
#include <sys/xattr.h>
#include <endian.h>
#endif
#include <sys/uio.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "ops.h"
#include "domains.h"
#include "domains_names.h"
#include "mq.h"
#include "io_function_types.h"
#include "io_monitor.h"


// TODO and enhancements
// - implement missing intercept calls (FILE_SPACE, PROCESSES, etc.)
// - find a better name/grouping for MISC
// - should there be a sampling mechanism? (capturing everything on a busy
//     server process can generate a lot of data)
// - implement missing functions for opening/creating files
//     http://man7.org/linux/man-pages/man2/open.2.html


static int failed_socket_connections = 0;
static int failed_ipc_sends = 0;

static key_t message_queue_key = -1;
static int message_queue_id = -1;
static unsigned int domain_bit_flags = 0;



//***********  initialization  ***********
void initialize_monitor();
unsigned int domain_list_to_bit_mask(const char* domain_list);

//***********  IPC mechanisms  ***********
int send_tcp_socket(struct monitor_record_t* monitor_record);
int send_msg_queue(struct monitor_record_t* monitor_record);

//***********  monitoring mechanism  ***********
void record(DOMAIN_TYPE dom_type,
            OP_TYPE op_type,
            int fd,
            const char* s1,
            const char* s2,
            struct timeval* start_time,
            struct timeval* end_time,
            int error_code,
            ssize_t bytes_transferred);
   

// unique identifier to know originator of metrics. defaults to 'u' (unspecified)
static char facility[5];
static const char* start_on_open = NULL;
static int socket_fd = -1;
static int paused = 0;
static int have_elapsed_threshold = 0;
static double elapsed_threshold = 0.0;


#include "io_function_orig_handlers.h"
void load_library_functions();

   

//*****************************************************************************

__attribute__((constructor)) void init() {
   PUTS("init");
   DECL_VARS()
   GET_START_TIME()
   CHECK_LOADED_FNS();
   /* retrieve actual command that was called */
   char cmdline[PATH_MAX];
   sprintf(cmdline, "/proc/%d/cmdline", getpid());
   int fd = orig_open(cmdline, O_RDONLY);
   int len = orig_read(fd, cmdline, PATH_MAX);
   if (len) {
     len--;
     while (--len) {
       if (!cmdline[len])
	 cmdline[len] = ' ';
     }
   } else {
     sprintf(cmdline, "could not determine path");
   }
   orig_close(fd);
   /* here retrieve actual path */

   GET_END_TIME();

   char ppid[10];
   sprintf(ppid, "%d", getppid());
   record(START_STOP, START, 0, cmdline, ppid,
          TIME_BEFORE(), TIME_AFTER(), 0, ZERO_BYTES);
   
}

//*****************************************************************************

__attribute__((destructor))  void fini() {
   PUTS("fini")
   DECL_VARS()
   GET_START_TIME()
   CHECK_LOADED_FNS();
   /* collect CPU usage, brk/heap size metrics from /proc */

   GET_END_TIME();

   
   record(START_STOP, STOP, 0, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), 0, ZERO_BYTES);
   //TODO: let collector know that we're done?
}

//*****************************************************************************

// we only need to load the original library functions once
void load_library_functions() {
   if (NULL != orig_open) {
      return;
   }

   start_on_open = getenv(ENV_START_ON_OPEN);
   const char* start_on_elapsed = getenv(ENV_START_ON_ELAPSED);
   if (start_on_open != NULL) {
      paused = 1;
   } else if (start_on_elapsed != NULL) {
      double elapsed_value = atof(start_on_elapsed);
      if (elapsed_value > 0.1) {
         elapsed_threshold = elapsed_value;
         have_elapsed_threshold = 1;
         paused = 1;
      }
   }

   // open/close
   orig_open = (orig_open_f_type)dlsym(RTLD_NEXT,"open");
   orig_open64 = (orig_open64_f_type)dlsym(RTLD_NEXT,"open64");
   orig_fopen = (orig_fopen_f_type)dlsym(RTLD_NEXT,"fopen");
   orig_fopen64 = (orig_fopen_f_type)dlsym(RTLD_NEXT,"fopen64");
   orig_creat = (orig_creat_f_type)dlsym(RTLD_NEXT,"creat");
   orig_creat64 = (orig_creat64_f_type)dlsym(RTLD_NEXT,"creat64");
   orig_close = (orig_close_f_type)dlsym(RTLD_NEXT,"close");
   orig_fclose = (orig_fclose_f_type)dlsym(RTLD_NEXT,"fclose");

   // write
   orig_write = (orig_write_f_type)dlsym(RTLD_NEXT,"write");
   orig_send = (orig_send_f_type)dlsym(RTLD_NEXT,"send");
   orig_pwrite = (orig_pwrite_f_type)dlsym(RTLD_NEXT,"pwrite");
   orig_writev = (orig_writev_f_type)dlsym(RTLD_NEXT,"writev");
   orig_pwritev = (orig_pwritev_f_type)dlsym(RTLD_NEXT,"pwritev");
   orig_fprintf = (orig_fprintf_f_type)dlsym(RTLD_NEXT,"fprintf");
   orig_vfprintf = (orig_vfprintf_f_type)dlsym(RTLD_NEXT,"vfprintf");
   orig_fwrite = (orig_fwrite_f_type)dlsym(RTLD_NEXT,"fwrite");

   // read
   orig_read = (orig_read_f_type)dlsym(RTLD_NEXT,"read");
   orig_recv = (orig_recv_f_type)dlsym(RTLD_NEXT,"recv");
   orig_pread = (orig_pread_f_type)dlsym(RTLD_NEXT,"pread");
   orig_readv = (orig_readv_f_type)dlsym(RTLD_NEXT,"readv");
   orig_preadv = (orig_preadv_f_type)dlsym(RTLD_NEXT,"preadv");
   orig_fread = (orig_fread_f_type)dlsym(RTLD_NEXT,"fread");
   orig_fscanf = (orig_fscanf_f_type)dlsym(RTLD_NEXT,"fscanf");
   orig_vfscanf = (orig_vfscanf_f_type)dlsym(RTLD_NEXT,"vfscanf");

   // sync
   orig_fsync = (orig_fsync_f_type)dlsym(RTLD_NEXT,"fsync");
   orig_fdatasync = (orig_fdatasync_f_type)dlsym(RTLD_NEXT,"fdatasync");
   orig_fflush = (orig_fflush_f_type)dlsym(RTLD_NEXT,"fflush");
   orig_sync = (orig_sync_f_type)dlsym(RTLD_NEXT,"sync");
   orig_syncfs = (orig_syncfs_f_type)dlsym(RTLD_NEXT,"syncfs");

   // xattrs
   orig_setxattr = (orig_setxattr_f_type)dlsym(RTLD_NEXT,"setxattr");
   orig_lsetxattr = (orig_lsetxattr_f_type)dlsym(RTLD_NEXT,"lsetxattr");
   orig_fsetxattr = (orig_fsetxattr_f_type)dlsym(RTLD_NEXT,"fsetxattr");
   orig_getxattr = (orig_getxattr_f_type)dlsym(RTLD_NEXT,"getxattr");
   orig_lgetxattr = (orig_lgetxattr_f_type)dlsym(RTLD_NEXT,"lgetxattr");
   orig_fgetxattr = (orig_fgetxattr_f_type)dlsym(RTLD_NEXT,"fgetxattr");
   orig_listxattr = (orig_listxattr_f_type)dlsym(RTLD_NEXT,"listxattr");
   orig_llistxattr = (orig_llistxattr_f_type)dlsym(RTLD_NEXT,"llistxattr");
   orig_flistxattr = (orig_flistxattr_f_type)dlsym(RTLD_NEXT,"flistxattr");
   orig_removexattr = (orig_removexattr_f_type)dlsym(RTLD_NEXT,"removexattr");
   orig_lremovexattr = (orig_lremovexattr_f_type)dlsym(RTLD_NEXT,"lremovexattr");
   orig_fremovexattr = (orig_fremovexattr_f_type)dlsym(RTLD_NEXT,"fremovexattr");

   // mount/umount
#ifndef __FreeBSD__
   orig_mount = (orig_mount_f_type)dlsym(RTLD_NEXT,"mount");
   orig_umount = (orig_umount_f_type)dlsym(RTLD_NEXT,"umount");
   orig_umount2 = (orig_umount2_f_type)dlsym(RTLD_NEXT,"umount2");
#endif

   // directory metadata
   orig_opendir = (orig_opendir_f_type)dlsym(RTLD_NEXT,"opendir");
   orig_fdopendir = (orig_fdopendir_f_type)dlsym(RTLD_NEXT,"fdopendir");
   orig_closedir = (orig_closedir_f_type)dlsym(RTLD_NEXT,"closedir");
   orig_readdir = (orig_readdir_f_type)dlsym(RTLD_NEXT,"readdir");
   orig_readdir_r = (orig_readdir_r_f_type)dlsym(RTLD_NEXT,"readdir_r");
   orig_dirfd = (orig_dirfd_f_type)dlsym(RTLD_NEXT,"dirfd");
   orig_rewinddir = (orig_rewinddir_f_type)dlsym(RTLD_NEXT,"rewinddir");
   orig_seekdir = (orig_seekdir_f_type)dlsym(RTLD_NEXT,"seekdir");
   orig_telldir = (orig_telldir_f_type)dlsym(RTLD_NEXT,"telldir");

   // file metadata
   orig_fstat = (orig_fstat_f_type)dlsym(RTLD_NEXT,"fstat");
   orig_lstat = (orig_lstat_f_type)dlsym(RTLD_NEXT,"lstat");
   orig_stat = (orig_stat_f_type)dlsym(RTLD_NEXT,"stat");
   orig_access = (orig_access_f_type)dlsym(RTLD_NEXT,"access");
   orig_faccessat = (orig_faccessat_f_type)dlsym(RTLD_NEXT,"faccessat");
   orig_chmod = (orig_chmod_f_type)dlsym(RTLD_NEXT,"chmod");
   orig_fchmod = (orig_fchmod_f_type)dlsym(RTLD_NEXT,"fchmod");
   orig_fchmodat = (orig_fchmodat_f_type)dlsym(RTLD_NEXT,"fchmodat");
   orig_chown = (orig_chown_f_type)dlsym(RTLD_NEXT,"chown");
   orig_fchown = (orig_fchown_f_type)dlsym(RTLD_NEXT,"fchown");
   orig_lchown = (orig_lchown_f_type)dlsym(RTLD_NEXT,"lchown");
   orig_fchownat = (orig_fchownat_f_type)dlsym(RTLD_NEXT,"fchownat");
   orig_utime = (orig_utime_f_type)dlsym(RTLD_NEXT,"utime");

   // allocate
   orig_posix_fallocate = (orig_posix_fallocate_f_type)dlsym(RTLD_NEXT,"posix_fallocate");
   orig_fallocate = (orig_fallocate_f_type)dlsym(RTLD_NEXT,"fallocate");

   // truncate
   orig_truncate = (orig_truncate_f_type)dlsym(RTLD_NEXT,"truncate");
   orig_ftruncate = (orig_ftruncate_f_type)dlsym(RTLD_NEXT,"ftruncate");

   // network
   orig_connect = (orig_connect_f_type)dlsym(RTLD_NEXT,"connect");
   orig_accept = (orig_accept_f_type)dlsym(RTLD_NEXT,"accept");
   orig_bind = (orig_bind_f_type)dlsym(RTLD_NEXT,"bind");
   orig_listen = (orig_listen_f_type)dlsym(RTLD_NEXT,"listen");
   orig_socket = (orig_socket_f_type)dlsym(RTLD_NEXT,"socket");

}

//*****************************************************************************

unsigned int domain_list_to_bit_mask(const char* domain_list)
{
   unsigned int bit_mask = 0;
   char* token;
   char* domain_list_copy = strdup(domain_list);
   char* rest = domain_list_copy;
   int i;
   
   while ((token = strtok_r(rest, ",", &rest))) {
     for (i = 0; i != END_DOMAINS; ++i) {
       if (!strcmp(token, domains_names[i])) {
	   bit_mask |= (1 << i);
	   break;
	 }
     }
   }
   free(domain_list_copy);

   return bit_mask;
}

//*****************************************************************************

void initialize_monitor() {
   // establish facility id
   memset(facility, 0, sizeof(facility));
   const char* facility_id = getenv(ENV_FACILITY_ID);
   if (facility_id != NULL) {
      strncpy(facility, facility_id, 4);
   } else {
      facility[0] = 'u';  // unspecified
   }

   message_queue_path = getenv(ENV_MESSAGE_QUEUE_PATH);

   const char* monitor_domain_list = getenv(ENV_MONITOR_DOMAINS);
   if (monitor_domain_list != NULL) {
      if (!strcmp(monitor_domain_list, "ALL")) {
         domain_bit_flags = -1; // turn all of them on
      } else {
         domain_bit_flags = domain_list_to_bit_mask(monitor_domain_list);
      }
   } else {
      // by default, don't record anything
      domain_bit_flags = 0;
   }

   load_library_functions();
}

//*****************************************************************************

int send_msg_queue(struct monitor_record_t* monitor_record)
{
   MONITOR_MESSAGE monitor_message;
   if (message_queue_key == MQ_KEY_NONE) {
      if (message_queue_path != NULL) {
         message_queue_key = ftok(message_queue_path, message_project_id);
         if (message_queue_key != -1) {
            message_queue_id = msgget(message_queue_key, 0600 | IPC_CREAT);
         }
      }
   }

   if (message_queue_id == MQ_KEY_NONE) {
      PUTS("no message queue available")
      return -1;
   }

   memset(&monitor_message, 0, sizeof(MONITOR_MESSAGE));
   monitor_message.message_type = 1L;
   memcpy(&monitor_message.monitor_record, monitor_record, sizeof (*monitor_record));

   return msgsnd(message_queue_id,
                 &monitor_message,
                 sizeof(*monitor_record),
                 IPC_NOWAIT);
}

//*****************************************************************************

int send_tcp_socket(struct monitor_record_t* monitor_record)
{
   int rc;
   int record_length;
   int sockfd;
   int port;
   char msg_size_header[10];
   struct sockaddr_in server;

   // set up a 10 byte header that includes the size (in bytes)
   // of our payload since sockets don't include any built-in
   // message boundaries
   record_length = sizeof(*monitor_record);
   memset(msg_size_header, 0, 10);
   snprintf(msg_size_header, 10, "%d", record_length);

   // we're using TCP sockets here to throw the record over the wall
   // to another process on same machine. TCP sockets is probably
   // not the best IPC mechanism for this purpose, but this is
   // handy to get started.
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd > 0) {
      port = SOCKET_PORT;

      // we DO NOT want to send anything remotely.
      // we are in the middle of the data path, so we need to
      // keep any induced latency to absolute minimum.
      server.sin_addr.s_addr = inet_addr("127.0.0.1");
      server.sin_family = AF_INET;
      server.sin_port = htons(port);
      socket_fd = sockfd;

      rc = connect(sockfd,
                   (struct sockaddr*) &server,
                   sizeof(server));
      if (rc == 0) {
         int one = 1;
         int send_buffer_size = 256;
#ifdef __FreeBSD__
         setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one));
#else
	 setsockopt(sockfd, SOL_TCP, TCP_NODELAY, &one, sizeof(one));
#endif
         setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF,
                    &send_buffer_size, sizeof(send_buffer_size));
         rc = write(sockfd, msg_size_header, 10);
         if (10 == rc) {
            rc = write(sockfd, monitor_record, record_length);
            if (record_length == rc) {
               rc = 0;
            } else {
               //printf("expected to write %d bytes, actual=%d\n",
               //       record_length, rc);
               rc = -1;
            }
         } else {
            //printf("header expected to write %d bytes, actual=%d\n", 10, rc);
            rc = -1;
         }
         //shutdown(sockfd, SHUT_RDWR);
      } else {
         // we're not able to reach our IPC peer. however, we're just a
         // thin shim used to intercept C library calls. we can't cause
         // the real process/application to crash or malfunction. the
         // show must go on...
         failed_socket_connections++;
         rc = -1;
      }
      close(sockfd);
      socket_fd = FD_NONE;
   }

   return rc;
}

//*****************************************************************************

#define RECORD_FIELD(f) record_output. f = f
#define RECORD_FIELD_S(f) if (f) {strncpy(record_output.f, f, sizeof(record_output.f)); \
    record_output.f[sizeof(record_output.f)-1] = 0; }

void record(DOMAIN_TYPE dom_type,
            OP_TYPE op_type,
            int fd,
            const char* s1,
            const char* s2,
            struct timeval* start_time,
            struct timeval* end_time,
            int error_code,
            ssize_t bytes_transferred)
{
   struct monitor_record_t record_output;
   unsigned long timestamp;
   int rc_ipc;
   int record_length;
   pid_t pid;
   double elapsed_time;

   // have we already tried to connect to our peer and failed?
   if (failed_socket_connections > 0) {
      // don't bother proceeding since we've already been unable to reach
      // the other side of our IPC. repeated failures just adds more
      // latency.
      return;
   }

   // exclude things that we should not be capturing
   // since we're using sockets, we're also intercepting
   // our own socket calls. if we're asked to record
   // any of our own work we just ignore it.
   if ((socket_fd != FD_NONE) && (fd == socket_fd)) {
      return;
   }

   // ignore reporting on stdin, stdout, stderr
   if ((fd > -1) && (fd < 3) && dom_type != START_STOP) {
      return;
   }

   
   // if we're not monitoring this domain we just ignore
   const unsigned int domain_bit_flag = 1 << dom_type;
   if (0 == (domain_bit_flags & domain_bit_flag)) {
      PUTS("ignoring domain")
      return;
   }

   if (op_type == OPEN) {
      if (!strcmp(s1, ".")) {
         // ignore open of current directory
         return;
      }

      if (!strcmp(s1, "..")) {
         // ignore open of parent directory
         return;
      }

      if (paused && (start_on_open != NULL) &&
          (strstr(s1, start_on_open) != NULL)) {
         PUTS("starting on open")
         paused = 0;
      }
   }

   // sec to msec
   elapsed_time = (end_time->tv_sec - start_time->tv_sec) * 1000.0;
   // usec to ms
   elapsed_time += (end_time->tv_usec - start_time->tv_usec) / 1000.0;

   if (paused && have_elapsed_threshold && (elapsed_time > elapsed_threshold)) {
      PUTS("starting on elapsed")
      paused = 0;
   }

   if (paused) {
      return;
   }

   timestamp = (unsigned long)time(NULL);
   pid = getpid();

   bzero(&record_output, sizeof(record_output));

   RECORD_FIELD_S(facility);
   RECORD_FIELD(timestamp);
   RECORD_FIELD(elapsed_time);
   RECORD_FIELD(pid);
   RECORD_FIELD(dom_type);
   RECORD_FIELD(op_type);
   RECORD_FIELD(error_code);
   RECORD_FIELD(fd);
   RECORD_FIELD(bytes_transferred);
   RECORD_FIELD_S(s1);
   RECORD_FIELD_S(s2);
   
   if (message_queue_path != NULL) {
      rc_ipc = send_msg_queue(&record_output);
   } else {
      rc_ipc = send_tcp_socket(&record_output);
   }

   if (rc_ipc != 0) {
      PUTS("io_monitor.c ipc send failed")
      failed_ipc_sends++;
   }
}

//*****************************************************************************

int open(const char* pathname, int flags, ...)
{
   CHECK_LOADED_FNS()
   PUTS("open")
   DECL_VARS()
   GET_START_TIME()
   const int fd = orig_open(pathname, flags);
   GET_END_TIME()

   if (fd == -1) {
      error_code = errno;
   }

   char* real_path = realpath(pathname, NULL);
   record(FILE_OPEN_CLOSE, OPEN, fd, real_path, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);
   free(real_path);

   return fd;
}

//*****************************************************************************

int open64(const char* pathname, int flags, ...)
{
   CHECK_LOADED_FNS()
   PUTS("open64")
   DECL_VARS()
   GET_START_TIME()
   const int fd = orig_open64(pathname, flags);
   GET_END_TIME()

   if (fd == -1) {
      error_code = errno;
   }

   char* real_path = realpath(pathname, NULL);
   record(FILE_OPEN_CLOSE, OPEN, fd, real_path, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);
   free(real_path);

   return fd;
}

//*****************************************************************************

int creat(const char* pathname, mode_t mode)
{
   CHECK_LOADED_FNS()
   PUTS("creat")
   DECL_VARS()
   GET_START_TIME()
   const int fd = orig_creat(pathname, mode);
   GET_END_TIME()

   if (fd == -1) {
      error_code = errno;
   }

   char* real_path = realpath(pathname, NULL);
   record(FILE_OPEN_CLOSE, OPEN, fd, real_path, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);
   free(real_path);

   return fd;
}

//*****************************************************************************

int creat64(const char* pathname, mode_t mode)
{
   CHECK_LOADED_FNS()
   PUTS("creat64")
   DECL_VARS()
   GET_START_TIME()
   const int fd = orig_creat64(pathname, mode);
   GET_END_TIME()

   if (fd == -1) {
      error_code = errno;
   }

   char* real_path = realpath(pathname, NULL);
   record(FILE_OPEN_CLOSE, OPEN, fd, real_path, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);
   free(real_path);

   return fd;
}

//*****************************************************************************

int close(int fd)
{
   CHECK_LOADED_FNS()
   PUTS("close")
   DECL_VARS()
   GET_START_TIME()
   const int rc = orig_close(fd);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(FILE_OPEN_CLOSE, CLOSE, fd, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

//*****************************************************************************

int fclose(FILE* fp)
{
   CHECK_LOADED_FNS()
   PUTS("fclose")
   DECL_VARS()
   GET_START_TIME()
   const int fd = fileno(fp);
   const int rc = orig_fclose(fp);
   GET_END_TIME()

   if (rc != 0) {
      error_code = errno;
   }

   record(FILE_OPEN_CLOSE, CLOSE, fd, NULL, NULL,
          TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);

   return rc;
}

