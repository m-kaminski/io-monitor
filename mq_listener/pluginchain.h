#ifndef __PLUGIN_CHAIN_H
#define __PLUGIN_CHAIN_H

typedef struct plugin_chain {
  const char* plugin_library;
  const char* plugin_options;
  const char* plugin_alias;   /* alternative method to access plugin in management routines */
  PFN_OPEN_PLUGIN pfn_open_plugin;
  PFN_CLOSE_PLUGIN pfn_close_plugin;
  PFN_OK_TO_ACCEPT_DATA pfn_ok_to_accept_data;
  PFN_PROCESS_DATA pfn_process_data;
  int plugin_paused;
  void* plugin_handle;
  struct plugin_chain* next_plugin;
};


int load_plugin(const char* library, const char* options, const char* alias)
{


}

#endif
