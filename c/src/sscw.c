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
#include <string.h>
#include <servicesync/moat.h>

#define TAG "SSCW"

#define UPLOAD_INTERVAL (60) /* sec */

typedef struct {
  sse_int user;
  sse_int nice;
  sse_int system;
  sse_int idle;
  sse_int iowait;
} CpuTickCount;

typedef struct {
  sse_float user;
  sse_float nice;
  sse_float system;
  sse_float idle;
  sse_float iowait;
} CpuUsage;

typedef struct {
  Moat moat;
  CpuTickCount last;
} UserContext;


static sse_int
get_cpu_usage(CpuTickCount* in_out_last, CpuUsage* out_cpu_usage)
{
  FILE* fp;
  sse_char buffer[512];
  sse_char* token;
  sse_char* delim = " ";
  sse_char* saveptr;
  sse_int count;
  CpuTickCount current;
  sse_int diff_user;
  sse_int diff_nice;
  sse_int diff_system;
  sse_int diff_idle;
  sse_int diff_iowait;
  sse_int total;

  sse_memset(&current, 0, sizeof(current));

  /* get cpu time with /proc/stat */
  if ((fp = fopen("/proc/stat", "r")) == NULL) {
    return SSE_E_GENERIC;
  }

  while (fgets(buffer, sizeof(buffer)-1, fp) != NULL) {
    /*
      $ cat /proc/stat
      cpu  314136 1037 108491 39186689 31126 10 5968 0 0 0
      cpu0 81224 1034 32204 9760195 26228 10 5611 0 0 0
      ... (snip) ...
    */
    for (count = 0, token = strtok_r(buffer, delim, &saveptr); token; token = strtok_r(NULL, delim, &saveptr)) {
      switch (count) {
      case 1: /* user */
        current.user = sse_atoi(token);
        break;
      case 2: /* nice */
        current.nice = sse_atoi(token);
        break;
      case 3: /* system */
        current.system = sse_atoi(token);
        break;
      case 4: /* idle */
        current.idle = sse_atoi(token);
        break;
      case 5: /* iowait */
        current.iowait = sse_atoi(token);
        break;
      default:
        ; /* nothing to do */
      }
      count++;      
    }
    break; /* just need to read a first line */
  }

  fclose(fp);

  /* Calculate a cpu usage */
  diff_user = current.user - in_out_last->user;
  diff_nice = current.nice - in_out_last->nice;
  diff_system = current.system - in_out_last->system;
  diff_idle = current.idle - in_out_last->idle;
  diff_iowait = current.iowait - in_out_last->iowait;
  total = diff_user + diff_nice + diff_system + diff_idle + diff_iowait;
  
  if (out_cpu_usage != NULL) {
    out_cpu_usage->user = (1.0f * diff_user / total) * 100.0f;
    out_cpu_usage->nice = (1.0f * diff_nice / total) * 100.0f;
    out_cpu_usage->system = (1.0f * diff_system / total) * 100.0f;
    out_cpu_usage->idle = (1.0f * diff_idle / total) * 100.0f;
    out_cpu_usage->iowait = (1.0f * diff_iowait / total) * 100.0f;
  }

  /* update the list tick count */
  in_out_last->user = current.user;
  in_out_last->nice = current.nice;
  in_out_last->system = current.system;
  in_out_last->idle = current.idle;
  in_out_last->iowait = current.iowait;

  return SSE_E_OK;
}

static void
upload_cpu_usage_result_proc(Moat in_moat, sse_char *in_urn, sse_char *in_model_name, sse_int in_request_id, sse_int in_result, sse_pointer in_user_data)
{
  if (in_result == SSE_E_OK) {
    MOAT_LOG_INFO(TAG, "moat_send_notificaion() has been complated.");
  } else {
    MOAT_LOG_INFO(TAG, "moat_send_notificaion() has failed with [%d].", in_result);
  }
}


static sse_bool
upload_cpu_usage(sse_int in_timer_id, sse_pointer in_user_data)
{
  sse_int err;
  sse_int request_id;
  MoatObject* object = NULL;
  CpuUsage cpu_usage;
  UserContext* ctx = (UserContext*)in_user_data;
  sse_char urn[512];

  err = get_cpu_usage(&(ctx->last), &cpu_usage);
  if (err != SSE_E_OK) {
    MOAT_LOG_ERROR(TAG, "get_cpu_usage() has failed with [%d].", err);
    return sse_true;
  }

  object = moat_object_new();
  if (object == NULL) {
    MOAT_LOG_ERROR(TAG, "moat_object_new() has failed.");
    return sse_true;
  }
  moat_object_add_float_value(object, "user", cpu_usage.user, sse_false);
  moat_object_add_float_value(object, "nice", cpu_usage.nice, sse_false);
  moat_object_add_float_value(object, "system", cpu_usage.system, sse_false);
  moat_object_add_float_value(object, "idle", cpu_usage.idle, sse_false);
  moat_object_add_float_value(object, "iowait", cpu_usage.iowait, sse_false);
  moat_object_add_int64_value(object, "timestamp", moat_get_timestamp_msec(), sse_false);

  snprintf(urn, sizeof(urn) - 1, "urn:moat:%s:upload-cpu-usage:1.0.0", moat_get_package_urn(ctx->moat));
  request_id = moat_send_notification(ctx->moat,                    /* Moat Instance */
                                      urn,                          /* URN */
                                      NULL,                         /* Key */
                                      "CpuUsage",                   /* Model name */
                                      object,                       /* Data collection */
                                      upload_cpu_usage_result_proc, /* Callback */
                                      ctx);                         /* User data */
  if (request_id < 0) {
    MOAT_LOG_ERROR(TAG, "moat_send_notification() has failed with [%d].", request_id);
  }

  moat_object_free(object);
  return sse_true;
}


sse_int
moat_app_main(sse_int in_argc, sse_char *argv[])
{
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
                            "CpuUsage",    /* Model name */
                            &model_mapper, /* ModelMapper instance */
                            ctx);          /* Context */
  if (err != SSE_E_OK) {
    goto error_exit;
  }

  /* set timer for monitoring cpu usage */
  get_cpu_usage(&(ctx->last), NULL); /* initialize last cpu count */  
  timer = moat_timer_new();
  if (timer == NULL) {
    goto error_exit;
  }
  timer_id = moat_timer_set(timer,            /* Timer instance */
                            UPLOAD_INTERVAL,  /* Interval sec   */
                            upload_cpu_usage, /* Callback       */
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
  moat_remove_model(moat, "CpuUsage");
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
