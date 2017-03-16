#ifndef PLUGIN_H
#define PLUGIN_H

#define PLUGIN_OPEN_SUCCESS  0
#define PLUGIN_OPEN_FAIL 1

#define PLUGIN_ACCEPT_DATA  1
#define PLUGIN_REFUSE_DATA  0

#include "monitor_record.h"

typedef int (*PFN_OPEN_PLUGIN)(const char*);
typedef void (*PFN_CLOSE_PLUGIN)();
typedef int (*PFN_OK_TO_ACCEPT_DATA)();
typedef int (*PFN_PROCESS_DATA)(struct monitor_record_t*);

/*
int open_plugin(const char* plugin_config);
void close_plugin();
int ok_to_accept_data();
int process_data(struct monitor_record_t* data);
*/

#endif

