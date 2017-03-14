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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include "domains.h"
#include "ops.h"
#include "ops_names.h"
#include "domains_names.h"
#include "mq.h"
#include "plugin.h"


static const int MESSAGE_QUEUE_PROJECT_ID = 'm';


//*****************************************************************************

void print_log_entry_formatted(struct monitor_record_t *data)
{
  static int ln=0;
  if (!((ln)&3))
    puts(""); /* print extra blank line every fourth line */


  if (!((ln++)&15)) {
    /* print header every 16th line"*/
    printf("%10s %10s %8s %5s %20s  %-20s %3s %5s %8s %s\n",
	   "FACILITY", "TS.", "ELAPSED",
	   "PID", "DOMAIN", "OPERATION", "ERR", "FD",
	   "XFER", "PARM");
  }
 
  printf("%10s %10d %8.4f %5d %20s  %-20s %3d %5d %8zu %s %s\n",
	 data->facility,
	 data->timestamp,
	 data->elapsed_time,
	 data->pid,
	 domains_names[data->dom_type],
	 ops_names[data->op_type], data->error_code, data->fd,
	 data->bytes_transferred, data->s1, data->s2);
}

//*****************************************************************************

void print_log_entry_csv(struct monitor_record_t* data)
{
  printf("%s,%d,%f,%d,%s,%s,%d,%d,%zu,%s,%s\n",
         data->facility,
         data->timestamp,
         data->elapsed_time,
         data->pid,
         domains_names[data->dom_type],
         ops_names[data->op_type], data->error_code, data->fd,
         data->bytes_transferred, data->s1, data->s2);
}

//*****************************************************************************

void show_usage_and_exit(const char* arg0, const char* error_msg)
{
   if (error_msg != NULL) {
      printf("error: %s\n", error_msg);
   }

   printf("usage: %s <msg-queue-path> [options]\n", arg0);
   printf("options:\n");
   printf("\t--csv - print output in csv format\n");
   printf("\t--plugin <plugin-library> [plugin-options]\n");
   exit(1);
}

//*****************************************************************************

int main(int argc, char* argv[])
{
   const char* message_queue_path;
   const char* plugin_library = NULL;
   const char* plugin_options = NULL;
   PFN_OPEN_PLUGIN pfn_open_plugin = NULL;
   PFN_CLOSE_PLUGIN pfn_close_plugin = NULL;
   PFN_OK_TO_ACCEPT_DATA pfn_ok_to_accept_data = NULL;
   PFN_PROCESS_DATA pfn_process_data = NULL;
   int plugin_paused = 0;
   void* plugin_handle = NULL;
   int message_queue_key;
   int message_queue_id;
   int rc;
   int csv_mode = 0;
   int plugin_mode = 0;
   int rc_plugin;
   ssize_t message_size_received;
   MONITOR_MESSAGE monitor_message;

   if (argc < 2) {
      show_usage_and_exit(argv[0], "missing arguments");
   }

   message_queue_path = argv[1];

   if (argc > 2) {
      if (!strcmp(argv[2], "--csv")) {
         csv_mode = 1;
      } else if (!strcmp(argv[2], "--plugin")) {
         if (argc < 4) {
            show_usage_and_exit(argv[0], "missing plugin library");
         } else {
            plugin_library = argv[3];
            if (argc > 4) {
               plugin_options = argv[4];
            }
            plugin_handle = dlopen(plugin_library, RTLD_NOW);
            if (NULL == plugin_handle) {
               printf("error: unable to open plugin library '%s'\n",
                      plugin_library);
               exit(1);
            }

            pfn_open_plugin =
               (PFN_OPEN_PLUGIN) dlsym(plugin_handle, "open_plugin");
            pfn_close_plugin =
               (PFN_CLOSE_PLUGIN) dlsym(plugin_handle, "close_plugin");
            pfn_ok_to_accept_data =
               (PFN_OK_TO_ACCEPT_DATA) dlsym(plugin_handle, "ok_to_accept_data");
            pfn_process_data =
               (PFN_PROCESS_DATA) dlsym(plugin_handle, "process_data"); 

            if ((NULL == pfn_open_plugin) ||
                (NULL == pfn_close_plugin) ||
                (NULL == pfn_ok_to_accept_data) ||
                (NULL == pfn_process_data)) {
               dlclose(plugin_handle);
               printf("error: plugin missing 1 or more entry points\n");
               exit(1);
            }

            rc_plugin = (*pfn_open_plugin)(plugin_options);
            if (rc_plugin == PLUGIN_OPEN_FAIL) {
               dlclose(plugin_handle);
               printf("error: unable to initialize plugin\n");
               exit(1);
            }
            plugin_mode = 1;
         }
      } else {
         show_usage_and_exit(argv[0], "unrecognized option");
      }
   }

   message_queue_key = ftok(message_queue_path, MESSAGE_QUEUE_PROJECT_ID);
   if (message_queue_key == -1) {
      printf("error: unable to obtain key for message queue path '%s'\n",
             message_queue_path);
      printf("errno: %d\n", errno);
      exit(1);
   }

   message_queue_id = msgget(message_queue_key, (0664 | IPC_CREAT));
   if (message_queue_id == -1) {
      printf("error: unable to obtain id for message queue path '%s'\n",
             message_queue_path);
      printf("errno: %d\n", errno);
      exit(1);
   }

   while (1) {
      memset(&monitor_message, 0, sizeof(MONITOR_MESSAGE));
      message_size_received =
         msgrcv(message_queue_id,
                &monitor_message,  // void* ptr
                sizeof(struct monitor_record_t),  // size_t nbytes
                0,   // long type
                0);  // int flag
      if (message_size_received > 0) {
         if (plugin_mode) {
            if (plugin_paused) {
               rc_plugin = (*pfn_ok_to_accept_data)();
               if (rc_plugin == PLUGIN_ACCEPT_DATA) {
                  plugin_paused = 0;
               }
            }

            if (!plugin_paused) {
               rc_plugin = (*pfn_process_data)(&monitor_message.monitor_record);
               if (rc_plugin == PLUGIN_REFUSE_DATA) {
                  plugin_paused = 1;
               }
            }
         } else if (csv_mode) {
            print_log_entry_csv(&monitor_message.monitor_record);
         } else {
            print_log_entry_formatted(&monitor_message.monitor_record);
         }
      } else {
         printf("rc = %zu\n", message_size_received);
         printf("errno = %d\n", errno);
      }
   }

   if (plugin_mode && (plugin_handle != NULL)) {
      (*pfn_close_plugin)();
      dlclose(plugin_handle);
      plugin_handle = NULL;
   }

   return 0;
}

//*****************************************************************************

