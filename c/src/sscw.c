/*
 * LEGAL NOTICE
 *
 * Copyright (C) 2012-2015 InventIt Inc. All rights reserved.
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
#include <stdlib.h>
#include <string.h>
#include <servicesync/moat.h>
  
#define TAG "SSCW"
#define UPLOAD_INTERVAL (60) /* sec */
#define MODEL_NAME "SensingData"
  
typedef struct {
  sse_float temperature;
  sse_float humidity;
} SensingData;
  
typedef struct {
  Moat moat;
} UserContext;
  
static void
get_sensor_data(SensingData* out_sensor_data){
  /* Temperature changes 0 - 40 */
  out_sensor_data->temperature = rand() % 41;
  
  /* Humidity changes in 0 - 100 */
  out_sensor_data->humidity = rand() % 101;
}
  
static void
upload_sensor_data_result_proc(Moat in_moat, sse_char *in_urn, sse_char *in_model_name, sse_int in_request_id, sse_int in_result, sse_pointer in_user_data){
  if (in_result == SSE_E_OK) {
    MOAT_LOG_INFO(TAG, "moat_send_notificaion() has been complated.");
  } else {
    MOAT_LOG_INFO(TAG, "moat_send_notificaion() has failed with [%d].", in_result);
  }
}
  
  
static sse_bool
upload_sensor_data(sse_int in_timer_id, sse_pointer in_user_data){
  sse_int urn_err;
  sse_int request_id;
  MoatObject* object = NULL;
  SensingData sensor_data;
  UserContext* ctx = (UserContext*)in_user_data;
  sse_char urn[512];
      
  get_sensor_data(&sensor_data);
      
  object = moat_object_new();
  if (object == NULL) {
    MOAT_LOG_ERROR(TAG, "moat_object_new() has failed.");
    return sse_true;
  }
      
  moat_object_add_float_value(object, "temperature", sensor_data.temperature, sse_false);
  moat_object_add_float_value(object, "humidity", sensor_data.humidity, sse_false);
  moat_object_add_int64_value(object, "timestamp", moat_get_timestamp_msec(), sse_false);
      
  urn_err = snprintf(urn, sizeof(urn) - 1, "urn:moat:%s:upload-sensing-data:1.0.0", moat_get_package_urn(ctx->moat));
  if (urn_err < 0 || urn_err >= sizeof(urn)) {
    MOAT_LOG_ERROR(TAG, "urn is unexpected size. maximum urn size is %d.", siezeof(urn));
    return sse_false;
  }
      
  request_id = moat_send_notification(ctx->moat,                    /* Moat Instance */
                                      urn,                          /* URN */
                                      NULL,                         /* Key */
                                      MODEL_NAME,                   /* Model name */
                                      object,                       /* Data collection */
                                      upload_sensor_data_result_proc, /* Callback */
                                      ctx);                         /* User data */
  if (request_id < 0) {
    MOAT_LOG_ERROR(TAG, "moat_send_notification() has failed with [%d].", request_id);
  }
      
  moat_object_free(object);
  return sse_true;
}
  
  
sse_int
moat_app_main(sse_int in_argc, sse_char *argv[]){
  Moat moat = NULL;
  MoatTimer* timer = NULL;
  sse_int timer_id;
  ModelMapper model_mapper;
  UserContext* ctx;
  sse_int err = SSE_E_OK;
      
  err = moat_init(argv[0], &moat);
  if (err != SSE_E_OK) {
    goto error_exit;
  }
      
  ctx = sse_zeroalloc(sizeof(UserContext));
  ctx->moat = moat;
      
  /* register models */
  sse_memset(&model_mapper, 0, sizeof(model_mapper));
  err = moat_register_model(moat,          /* MOAT instance */
                            MODEL_NAME,    /* Model name */
                            &model_mapper, /* ModelMapper instance */
                            ctx);          /* Context */
  if (err != SSE_E_OK) {
    goto error_exit;
  }
      
  /* set timer for monitoring sensing data */
  timer = moat_timer_new();
  if (timer == NULL) {
    goto error_exit;
  }
  timer_id = moat_timer_set(timer,            /* Timer instance */
                            UPLOAD_INTERVAL,  /* Interval sec   */
                            upload_sensor_data, /* Callback       */
                            ctx);             /* User data      */
  if (timer_id < 1) {
    goto error_exit;
  }
      
  /* main loop */
  err = moat_run(moat);
  if (err != SSE_E_OK) {
    MOAT_LOG_ERROR(TAG, "moat_run() has failed with [%d].", err);
  }
      
  /* Teardown */
  moat_timer_free(timer);
  moat_remove_model(moat, MODEL_NAME);
  sse_free(ctx);
  moat_destroy(moat);
      
  return SSE_E_OK;
      
  error_exit:
  if (timer != NULL) {
    moat_timer_free(timer);
  }
  if (moat != NULL) {
    moat_destroy(moat);
  }
  return err;
}
