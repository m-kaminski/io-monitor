#ifndef __PLUGIN_CHAIN_H
#define __PLUGIN_CHAIN_H

struct plugin_chain {
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

struct plugin_chain* plugins = NULL;

int execute_plugin_chain(struct monitor_record_t *rec)
{
  struct plugin_chain* p = plugins;
  int rc_plugin;
  while (p) {
  if (p->plugin_paused) {
    rc_plugin = p->pfn_ok_to_accept_data();
    if (rc_plugin == PLUGIN_ACCEPT_DATA) {
      p->plugin_paused = 0;
    }
  }

  if (!p->plugin_paused) {
    rc_plugin = p->pfn_process_data(rec);
    if (rc_plugin == PLUGIN_REFUSE_DATA) {
      p->plugin_paused = 1;
    }
  }
  p = p->next_plugin;
  }
  return 0;
}
void unload_all_plugins() {
   /* if (plugin_mode && (plugin_handle != NULL)) { */
   /*    (*pfn_close_plugin)(); */
   /*    dlclose(plugin_handle); */
   /*    plugin_handle = NULL; */
   /* } */
}
int load_plugin(const char* library, const char* options, const char* alias)
{
  struct plugin_chain* new_plugin = calloc(1, sizeof(struct plugin_chain));
  if (!new_plugin) {
    return 1;
  }
  new_plugin->plugin_library = library;
  new_plugin->plugin_options = options;
  new_plugin->plugin_handle = dlopen(new_plugin->plugin_library, RTLD_NOW);
  if (NULL == new_plugin->plugin_handle) {
    printf("error: unable to open plugin library '%s'\n",
	   new_plugin->plugin_library);
    return 1;
  }

  new_plugin->pfn_open_plugin =
    (PFN_OPEN_PLUGIN) dlsym(new_plugin->plugin_handle, "open_plugin");
  new_plugin->pfn_close_plugin =
    (PFN_CLOSE_PLUGIN) dlsym(new_plugin->plugin_handle, "close_plugin");
  new_plugin->pfn_ok_to_accept_data =
    (PFN_OK_TO_ACCEPT_DATA) dlsym(new_plugin->plugin_handle, "ok_to_accept_data");
  new_plugin->pfn_process_data =
    (PFN_PROCESS_DATA) dlsym(new_plugin->plugin_handle, "process_data"); 

  if ((NULL == new_plugin->pfn_open_plugin) ||
      (NULL == new_plugin->pfn_close_plugin) ||
      (NULL == new_plugin->pfn_ok_to_accept_data) ||
      (NULL == new_plugin->pfn_process_data)) {
    dlclose(new_plugin->plugin_handle);
    printf("error: plugin missing 1 or more entry points\n");
    return 1;
  }

  int rc_plugin = (*new_plugin->pfn_open_plugin)(new_plugin->plugin_options);
  if (rc_plugin == PLUGIN_OPEN_FAIL) {
    dlclose(new_plugin->plugin_handle);
    printf("error: unable to initialize plugin\n");
    return 1;
  }

  plugins=new_plugin;
}

#endif
