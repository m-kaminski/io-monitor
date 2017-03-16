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

#ifndef __MQ_H
#define __MQ_H
#include "monitor_record.h"


typedef struct _MONITOR_MESSAGE
{
   long message_type;
   struct monitor_record_t monitor_record;
} MONITOR_MESSAGE;

#endif //__MQ_H
