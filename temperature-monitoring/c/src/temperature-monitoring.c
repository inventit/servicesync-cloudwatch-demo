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

#include <linux/input.h>
#include <servicesync/moat.h>
#include "switch.h"
#include "sensor.h"
#include "uploader.h"
#include "led.h"

#define TAG "Sample"

struct _Context {
  Moat moat;
  Sensor* sensor;
  SensorListener* sensor_listener;
  Switch* user_switch;
  SwitchEventListener* switch_listener;
  Uploader* uploader;
};
typedef struct _Context Context;

static sse_int
on_key_pressed(sse_pointer in_user_data)
{
  sse_int err;
  Context* context = (Context *)in_user_data;
  Sensor* sensor = (Sensor*)(context->sensor);
  SensorListener* listener = (SensorListener*)(context->sensor_listener);

  if (sensor->timer_id < 1) {
    SSE_LOG_INFO(TAG, "Sensing start");
    err = Sensor_subscribe(sensor, listener);
  } else {
    SSE_LOG_INFO(TAG, "Sensing stop");
    err = Sensor_unsubscribe(sensor, listener);
  }

  return err;
}

static sse_int
on_sensing_data(sse_int32 in_temperature, sse_pointer in_user_data)
{
  Context* context = (Context *)in_user_data;
  MoatObject* object;

  SSE_LOG_INFO(TAG, "Temperature = %d", in_temperature);

  object = moat_object_new();
  moat_object_add_int32_value(object, "temperature", in_temperature, sse_false);
  moat_object_add_int64_value(object, "timestamp", moat_get_timestamp_msec(), sse_false);
  Uploader_post(context->uploader, object);

  return SSE_E_OK;
}

sse_int
SensingData_startstop(Moat in_moat, sse_char *in_uid, sse_char *in_key, MoatValue *in_data, sse_pointer in_model_context)
{
  sse_int err;
  sse_char* option;
  sse_uint len;
  Context* context = (Context *)in_model_context;
  Sensor* sensor = (Sensor*)(context->sensor);
  SensorListener* listener = (SensorListener*)(context->sensor_listener);

  err = moat_value_get_string(in_data, &option, &len);
  if (err != SSE_E_OK) {
    SSE_LOG_ERROR(TAG, "() ... failed with [%d].", err);
    return err;
  }
  SSE_LOG_INFO(TAG, "Modle:SensingData - Command:startstop [%s] is executed.", option);
  
  if (sse_strcmp(option, "start") == 0) {
    if (sensor->timer_id < 1) {
      SSE_LOG_INFO(TAG, "Sensing start");
      Sensor_subscribe(sensor, listener);
    } else {
      SSE_LOG_INFO(TAG, "Sensing is already started");
    }
  } else if (sse_strcmp(option, "stop") == 0) {
    if (sensor->timer_id >= 1) {
      SSE_LOG_INFO(TAG, "Sensing stop");
      Sensor_unsubscribe(sensor, listener);
    } else {
      SSE_LOG_INFO(TAG, "Sensing is already stopped");
    }
  } else {
    SSE_LOG_ERROR(TAG, "Unknown option = [%s].", option);
  }

  return SSE_E_OK;
}

sse_int
moat_app_main(sse_int in_argc, sse_char *argv[])
{
  Moat moat = NULL;
  ModelMapper model_mapper;
  Context context;
  sse_int err = SSE_E_OK;

  SSE_LOG_INFO(TAG, "Start.");

  LED_init();
  LED_turn_off(LED1);
  LED_turn_off(LED2);
  LED_turn_off(LED3);
  LED_turn_off(LED4);

  err = moat_init(argv[0], &moat);
  if (err != SSE_E_OK) {
    goto error_exit;
  }
  context.moat = moat;

  /* register models */
  sse_memset(&model_mapper, 0, sizeof(model_mapper));
  err = moat_register_model(moat,          /* MOAT instance */
                            "SensingData", /* Model name */
                            &model_mapper, /* ModelMapper instance */
                            &context);     /* Context */
  if (err != SSE_E_OK) {
    SSE_LOG_ERROR(TAG, "moat_register_model() ... failed with [%d].", err);
    goto error_exit;
  }

  /* temperature sensor */
  context.sensor = Sensor_new();
  if (context.sensor == NULL) {
    err = SSE_E_NOMEM;
    goto error_exit;
  }
  Sensor_set_interval(context.sensor, 10);
  context.sensor_listener = SensorListener_new(&context, on_sensing_data);

  /* user switch */
  context.user_switch = Switch_new("/dev/input/event0");
  if (context.user_switch == NULL) {
    err = SSE_E_NOMEM;
    goto error_exit;
  }
  context.switch_listener = SwitchEventListener_new(EV_KEY, KEY_1, 1, &context, on_key_pressed);
  if (context.switch_listener == NULL) {
    err = SSE_E_NOMEM;
    goto error_exit;
  }
  Switch_add_listener(context.user_switch, context.switch_listener);

  /* uploader */
  context.uploader = Uploader_new(moat, "SensingData");
  if (context.uploader == NULL) {
    err = SSE_E_NOMEM;
    goto error_exit;
  }
  Uploader_start(context.uploader);

  /* main loop */
  LED_turn_on(LED2);
  err = moat_run(moat);
  if (err != SSE_E_OK) {
    goto error_exit;
  }
  LED_turn_off(LED2);


  /* teardown */
  Sensor_unsubscribe(context.sensor, context.sensor_listener);
  Sensor_free(context.sensor);
  Switch_free(context.user_switch);

  /* unregister models */
  moat_remove_model(moat, "SensingData");
  moat_destroy(moat);
  LED_teardown();
  SSE_LOG_INFO(TAG, "Teardown.");
  return SSE_E_OK;

 error_exit:
  if (moat != NULL) {
    moat_destroy(moat);
  }
  return err;
}
