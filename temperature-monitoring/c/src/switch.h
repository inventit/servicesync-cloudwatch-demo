/*
 * LEGAL NOTICE
 *
 * Copyright (C) 2012-2013 InventIt Inc. All rights reserved.
 *
 * This source code, product and/or document is protected under licenses
 * restricting its use, copying, distribution, and decompilation.
 * No part of this source code, product or document may be reproduced in
 * any form by any means without prior written authorization of InventIt Inc.
 * and its licensors, if any.
 *
 * InventIt Inc.
 * 9F KOJIMACHI CP BUILDING
 * 4-4-7 Kojimachi, Chiyoda-ku, Tokyo 102-0083
 * JAPAN
 * http://www.yourinventit.com/
 */

#ifndef __SWITCH_H__
#define __SWITCH_H__

#ifdef __cplusplus
extern "C" {
#endif

struct _Switch {
  MoatIOWatcher* watcher;
  SSEList* listeners;
};
typedef struct _Switch Switch;

typedef sse_int (*SwitchEventCallback)(sse_pointer user_data);

struct _SwitchEventListener {
  sse_uint16 type;
  sse_uint16 code;
  sse_int32 value;
  sse_pointer user_data;
  SwitchEventCallback callback;
};
typedef struct _SwitchEventListener SwitchEventListener;

Switch* Switch_new(const sse_char* in_event);
void Switch_free(Switch* in_this);
sse_int Switch_add_listener(Switch* in_this, SwitchEventListener* in_listener);
sse_int Switch_remove_listener(Switch* in_this, SwitchEventListener* in_listener);
sse_int Switch_activate(Switch* in_this, sse_char* in_event);
sse_int Switch_inactivate(Switch* in_this, sse_pointer user_data);

SwitchEventListener* SwitchEventListener_new(sse_uint16 in_type, sse_uint16 in_code, sse_int32 in_value, sse_pointer in_user_data, SwitchEventCallback in_callback);
void SwitchEventListener_free(SwitchEventListener* in_this);

#ifdef __cplusplus
}
#endif

#endif /* __SWITCH_H__ */

