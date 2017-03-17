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
#include <unistd.h>
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
#include "plugin_chain.h"

static const int MESSAGE_QUEUE_PROJECT_ID = 'm';

void show_usage_and_exit(const char* arg0, const char* error_msg)
{
   if (error_msg != NULL) {
      printf("error: %s\n", error_msg);
   }

   printf("usage: %s <msg-queue-path> [options]\n", arg0);
   printf("options:\n");
   printf("\t--plugin <plugin-library> [plugin-options]\n");
   exit(1);
}

//*****************************************************************************

int main(int argc, char* argv[])
{
   const char* message_queue_path;

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
	     const char* plugin_library = 0;
	     const char* plugin_options = 0;
	     plugin_library = argv[3];
	     if (argc > 4) {
	       plugin_options = argv[4];
	     }
	     if (!load_plugin(plugin_library, plugin_options, NULL)) {
	       /* plugin successfully loaded */	     
	     plugin_mode = 1;
	   }
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
	execute_plugin_chain(&monitor_message.monitor_record);
      } else {
         printf("rc = %zu\n", message_size_received);
         printf("errno = %d\n", errno);
      }
   }

   unload_all_plugins();

   return 0;
}

//*****************************************************************************

