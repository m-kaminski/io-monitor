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

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "plugin.h"
#include "monitor_record.h"
#include "domains.h"
#include "ops.h"
#include "domains_names.h"
#include "ops_names.h"
#include "utility_routines.h"

//*****************************************************************************
unsigned int domain_bit_flags = -1;

int open_plugin(const char* plugin_config)
{
  domain_bit_flags=domain_list_to_bit_mask(plugin_config);
   return PLUGIN_OPEN_SUCCESS;
}

//*****************************************************************************

void close_plugin()
{
}

//*****************************************************************************

int ok_to_accept_data()
{
   return PLUGIN_ACCEPT_DATA;
}

//*****************************************************************************

int process_data(struct monitor_record_t* data)
{
   const unsigned int domain_bit_flag = 1 << data->dom_type;
   if (0 == (domain_bit_flags & domain_bit_flag)) {
      return PLUGIN_DROP_DATA;
   } else { 
     return PLUGIN_ACCEPT_DATA;
   }
}

//*****************************************************************************

