#ifndef PLUGIN_H
#define PLUGIN_H

#define PLUGIN_OPEN_SUCCESS  0
#define PLUGIN_OPEN_FAIL 1

/*
 * default answer for PFN_PROCESS_DATA (data was written/passed/otherwise
 * accepted 
 */
#define PLUGIN_ACCEPT_DATA  1

/*
 * plugin did not process data i.e. because it's a plugin dumping data
 * to remote database and database is busy/offline. Doesn't cause data
 * to be dropped from other plugins
 */
#define PLUGIN_REFUSE_DATA  0

/*
 * drop data - response issued typically by filter type plugins. causes
 * data not to be passed to plugins loaded after this one.
 */
#define PLUGIN_DROP_DATA   -1

#include "monitor_record.h"
/* structure passed to open function - passes parameters
 * containing state of mq_listener program, including
 * handles allowing interaction with it */
struct listener {
  int (*command_function)(const char* buf);
};
/* plugin needs to expose at least following four functions */

typedef int (*PFN_OPEN_PLUGIN)(const char*, struct listener *);
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

