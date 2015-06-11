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

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <servicesync/moat.h>
#include "switch.h"

#define TAG "Sample:Switch"

static void
Switch_on_read(MoatIOWatcher *in_watcher, sse_pointer in_user_data, sse_int in_desc, sse_int in_event_flags)
{
  sse_int fd;
  struct input_event event;
  int ret;
  Switch* this = (Switch*)in_user_data;
  SSEList* it;
  SwitchEventListener* listener;

  fd = moat_io_watcher_get_descriptor(in_watcher);

  if (in_event_flags & MOAT_IO_FLAG_READ) {
    ret = read(fd, &event, sizeof(event));
    if (ret != sizeof(event)) {
      if (ret < 0) {
	sse_int err_no = errno;
	SSE_LOG_ERROR(TAG, "read() ... failed with [%s]", strerror(err_no));
      } else {
	SSE_LOG_ERROR(TAG, "read length was too short or long. [expected = %d / actual = %d]", sizeof(event), ret);
      } 
      return;
    }

    for (it = this->listeners; it != NULL; it = sse_list_next(it)) {
      listener = sse_list_data(it);
      if (event.type == listener->type && event.code == listener->code && event.value == listener->value){
	listener->callback(listener->user_data);
      }
    }
  }
}


Switch*
Switch_new(const sse_char* in_event)
{
  sse_int fd;
  sse_int err;
  Switch* this;

  this = sse_zeroalloc(sizeof(Switch));
  if (this == NULL) {
    SSE_LOG_ERROR(TAG, "sse_zeroalloc() ... failed.");
    goto error_exit;
  }

  this->watcher = moat_io_watcher_new(0, NULL, this, 0);
  if (this->watcher == NULL) {
    SSE_LOG_ERROR(TAG, "moat_io_watcher_new() ... failed.");
    goto error_exit;
  }

  fd = open(in_event, O_RDONLY);
  if (fd < 0) {
    sse_int err_no = errno;
    SSE_LOG_ERROR(TAG, "open() ... failed with [%s].", strerror(err_no));
    goto error_exit;
  }
  moat_io_watcher_set_descriptor(this->watcher, fd, MOAT_IO_FLAG_READ);
  moat_io_watcher_set_handler(this->watcher, Switch_on_read, this);
  err = moat_io_watcher_start(this->watcher);
  if (err != SSE_E_OK) {
    SSE_LOG_ERROR(TAG, "moat_io_watcher_start() ... failed with [%d].", err);
    close(fd);
    goto error_exit;
  }

  return this;

 error_exit:
  if (this != NULL) {
    Switch_free(this);
  }
  return NULL;
}

void
Switch_free(Switch* in_this)
{
  if (in_this != NULL) {
    if (in_this->watcher != NULL) {
      moat_io_watcher_stop(in_this->watcher);
      moat_io_watcher_free(in_this->watcher);
    }
    if (in_this->listeners != NULL) {
      sse_list_free(in_this->listeners);
    }
    sse_free(in_this);
  }
}

sse_int
Switch_add_listener(Switch* in_this, SwitchEventListener* in_listener)
{
  in_this->listeners = sse_list_add(in_this->listeners, in_listener);
  return SSE_E_OK;
}

sse_int
Switch_remove_listener(Switch* in_this, SwitchEventListener* in_listener)
{
  in_this->listeners = sse_list_remove(in_this->listeners, in_listener);
  return SSE_E_OK;
}

SwitchEventListener*
SwitchEventListener_new(sse_uint16 in_type, sse_uint16 in_code, sse_int32 in_value, sse_pointer in_user_data, SwitchEventCallback in_callback)
{
  SwitchEventListener* this = sse_zeroalloc(sizeof(SwitchEventListener));
  if (this == NULL) {
    SSE_LOG_ERROR(TAG, "sse_zeroalloc() ... failed");
    return NULL;
  }

  this->type = in_type;
  this->code = in_code;
  this->value = in_value;
  this->user_data = in_user_data;
  this->callback = in_callback;

  return this;
}

void SwitchEventListener_free(SwitchEventListener* in_this)
{
  sse_free(in_this);
}
