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
#include "io_function_types.h"
#include "io_function_orig_handlers.h"

char *real_ip(const struct sockaddr *addr, char *out)
{
   /* for now assume that addr->sa_family = AF_INET; for inet6 or other sockets,
      different way of differentiating will be needed */
   if (addr->sa_family != AF_INET) {
     PUTS("Warn: connect to addresses other than AF_INET won't work with current gen of io_monitor");
     return 0 ;
   }
   struct sockaddr_in * ai = (((struct sockaddr_in*)(addr)));
   char* real_path = malloc(100);
   char* ip = (char*)&ai->sin_addr;

   sprintf(&real_path[0], "%u.%u.%u.%u:%u" ,
	   0xff& ip[0], 0xff& ip[1] ,0xff& ip[2] ,0xff& ip[3],
	   be16toh(ai->sin_port));
   return real_path;
}

#include "intercept_functions.h"
