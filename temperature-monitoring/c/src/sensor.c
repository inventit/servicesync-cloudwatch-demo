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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <servicesync/moat.h>
#include "sensor.h"
#include "led.h"

#define TAG "Sample:Sensor"

static const sse_int SensorDefaultInterval = 10;
static const sse_char* SensorDefaultEventFile = "/sys/devices/platform/i2c-gpio.3/i2c-adapter/i2c-3/3-0048/temp1_input";

static sse_int
sensor_read_temperature(const sse_char *in_path, sse_int32* out_temperature)
{
  FILE* fd;
  sse_char buffer[128];
  sse_int temperature = 0;

  if ((fd = fopen(in_path, "r")) == NULL) {
    int err_no = errno;
    SSE_LOG_ERROR(TAG, "foepn() ... failed with %s", strerror(err_no));
    return SSE_E_GENERIC;
  }

  sse_memset(buffer, 0, sizeof(buffer));
  while (fgets(buffer, sizeof(buffer)-1, fd) != NULL) {
    buffer[sse_strlen(buffer) - 1] = '\0';
    temperature = sse_atoi(buffer);
    break;
  }
  fclose(fd);

  *out_temperature = temperature;
  return SSE_E_OK;
}

static sse_bool
sensor_on_timer_event(sse_int in_timer_id, sse_pointer in_user_data)
{
  sse_int err;
  sse_int32 temperature;
  Sensor* this = (Sensor *)in_user_data;
  SSEList* it;
  SensorListener* listener;

  err = sensor_read_temperature(this->event_file, &temperature);
  if (err != SSE_E_OK) {
    SSE_LOG_ERROR(TAG, "getting temperature failed.");
    return sse_true;
  }
  SSE_LOG_DEBUG(TAG, "Temperature = %d", temperature);

  for (it = this->listeners; it != NULL; it = sse_list_next(it)) {
    listener = sse_list_data(it);
    listener->callback(temperature, listener->user_data);
  }

  return sse_true;
}

Sensor*
Sensor_new()
{
  sse_char *event_file;

  Sensor* this = sse_zeroalloc(sizeof(Sensor));
  if (this == NULL) {
    SSE_LOG_ERROR(TAG, "sse_zeroalloc() ... failed.");
    return NULL;
  }

  this->timer = moat_timer_new();
  if (this->timer == NULL) {
    SSE_LOG_ERROR(TAG, "moat_timer_new() ... failed.");
    goto error_exit;
  }
  this->interval = SensorDefaultInterval;

  event_file = getenv("SSEGW_SAMPLE_TEMP_SENSOR");
  if (event_file == NULL) {
    this->event_file = sse_strdup(SensorDefaultEventFile);
  } else {
    this->event_file = sse_strdup(event_file);
  }
  if (this->event_file == NULL) {
    SSE_LOG_ERROR(TAG, "moat_timer_new() ... failed.");
    goto error_exit;
  }
  SSE_LOG_INFO(TAG, "Event file of Sensor=[%s]", this->event_file);

  return this;

 error_exit:

  if (this != NULL) {
    if (this->timer != NULL) moat_timer_free(this->timer);
    if (this->event_file !=NULL) sse_free(this->event_file);
    Sensor_free(this);
  }
  return NULL;
}

sse_int
Sensor_free(Sensor* in_this)
{
  if (in_this != NULL) {
    if (in_this->timer != NULL) moat_timer_free(in_this->timer);
    if (in_this->event_file !=NULL) sse_free(in_this->event_file);
    sse_free(in_this);
  }
  return SSE_E_OK;
}

sse_int
Sensor_subscribe(Sensor* in_this, SensorListener* in_listener)
{
  if (in_this->timer_id > 0) {
    SSE_LOG_ERROR(TAG, "tiemr has already started. id = [%d]", in_this->timer_id);
    return SSE_E_GENERIC;
  }

  if (in_this->listeners == NULL) {
    LED_turn_on(LED3);
    in_this->timer_id = moat_timer_set(in_this->timer,
				       in_this->interval,
				       sensor_on_timer_event,
				       in_this);
    if (in_this->timer_id < 0) {
      SSE_LOG_ERROR(TAG, "moat_timer_set() failed with [%d]", in_this->timer_id);
      return SSE_E_GENERIC;
    }
  }

  in_this->listeners = sse_list_add(in_this->listeners, in_listener);

  return SSE_E_OK;
}

sse_int
Sensor_unsubscribe(Sensor* in_this, SensorListener* in_listener)
{
  if (in_this->timer_id < 1) {
    SSE_LOG_ERROR(TAG, "Invalit timer id = [%d]", in_this->timer_id);
    return SSE_E_GENERIC;
  }
  
  in_this->listeners = sse_list_remove(in_this->listeners, in_listener);

  if ((sse_list_length(in_this->listeners) == 0) && (in_this->timer_id != 0)) {
    LED_turn_off(LED3);
    moat_timer_cancel(in_this->timer, in_this->timer_id);
    in_this->timer_id = 0;
  }

  return SSE_E_OK;
}

void
Sensor_set_interval(Sensor* in_this, sse_int in_interval)
{
  in_this->interval = in_interval;
}

sse_int
Sendor_get_interval(Sensor* in_this)
{
  return in_this->interval;
}

SensorListener*
SensorListener_new(sse_pointer user_data, SensorListenerCallback callback)
{
  SensorListener* this = sse_zeroalloc(sizeof(SensorListener));
  if (this == NULL) {
    SSE_LOG_ERROR(TAG, "sse_zeroalloc() failed.");
    return NULL;
  }

  this->user_data = user_data;
  this->callback = callback;
  
  return this;
}

void
SensorListener_free(SensorListener* in_this)
{
  sse_free(in_this);
}
