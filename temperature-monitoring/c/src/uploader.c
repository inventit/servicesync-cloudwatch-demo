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
#include <servicesync/moat.h>
#include "uploader.h"
#include "led.h"

#define TAG "Sample:Uploader"

static void
Uploader_result_proc(Moat in_moat, sse_char *in_urn, sse_char *in_model_name, sse_int in_request_id, sse_int in_result, sse_pointer in_user_data)
{
  if (in_result == SSE_E_OK) {
    SSE_LOG_INFO(TAG, "moat_send_notificaion(id = %d) has been complated.", in_request_id);
  } else {
    SSE_LOG_INFO(TAG, "moat_send_notificaion(id = %d) has failed with [%d].", in_request_id, in_result);
  }
  LED_turn_off(LED4);
}

static void
Uploader_periodic_proc(MoatPeriodic *in_periodic, sse_pointer in_user_data)
{
  sse_int request_id;
  Uploader* this = (Uploader *)in_user_data;
  MoatObject* sensing_data;
  MoatObject* collection;
  sse_char* uuid;
  sse_int err;
  sse_char urn[512];

  if (sse_queue_is_empty(this->queue)) {
    return;
  }

  collection = moat_object_new();
  if (collection == NULL) {
    SSE_LOG_ERROR(TAG, "moat_object_new() ... failed.");
    return;
  }

  while (!sse_queue_is_empty(this->queue)) {
    sensing_data = sse_queue_dequeue(this->queue);
    uuid = moat_uuid_create_string();

    err = moat_object_add_object_value(collection, uuid, sensing_data, sse_false, sse_false);
    if (err != SSE_E_OK) {
      SSE_LOG_ERROR(TAG, "moat_object_add_object_value() ... failed with [%d].", err);
      moat_object_free(collection);
      moat_object_free(sensing_data);
      sse_free(uuid);
      return;
    }
  }

  LED_turn_on(LED4);
  snprintf(urn, sizeof(urn) - 1, "urn:moat:%s:upload-temperature:1.0.0", moat_get_package_urn(this->moat));
  request_id = moat_send_notification(this->moat,
				      urn,
				      NULL,
				      this->model_name,
				      collection,
				      Uploader_result_proc,
				      this);
  if (request_id < 0) {
    SSE_LOG_INFO(TAG, "moat_send_notificaion(id = %d) is in progress.", request_id);
  } else {
    SSE_LOG_INFO(TAG, "moat_send_notificaion(id = %d) is in progress.", request_id);
  }
  moat_object_free(collection);

  return;
}

Uploader*
Uploader_new(Moat in_moat, const sse_char* in_model_name)
{
  Uploader* this;

  this = sse_zeroalloc(sizeof(Uploader));
  if (this == NULL) {
    SSE_LOG_ERROR(TAG, "sse_zeroalloc() ... failed.");
    goto error_exit;
  }

  this->moat = in_moat;

  this->model_name = sse_strdup(in_model_name);
  if (this->model_name == NULL) {
    SSE_LOG_ERROR(TAG, "sse_strdup() ... failed.");
    goto error_exit;
  }

  this->interval = UploaderDefaultInterval;

  this->periodic = moat_periodic_new(Uploader_periodic_proc, this, this->interval);
  if (this->periodic == NULL) {
    SSE_LOG_ERROR(TAG, "moat_periodic_new() ... failed.");
    goto error_exit;
  }

  this->queue = sse_queue_new();
  if (this->queue == NULL) {
    SSE_LOG_ERROR(TAG, "sse_queue_new() ... failed.");
    goto error_exit;
  }

  return this;

 error_exit:
  if (this != NULL) {
    Uploader_free(this);
  }
  return NULL;
}

void
Uploader_free(Uploader *in_this)
{
  if (in_this) {
    if (in_this->model_name != NULL) sse_free(in_this->model_name);
    if (in_this->periodic != NULL)   moat_periodic_free(in_this->periodic);
    if (in_this->queue != NULL)      sse_queue_free(in_this->queue);
    sse_free(in_this);
  }
}

sse_int
Uploader_start(Uploader *in_this)
{
  sse_int err;
  sse_bool is_active;

  is_active = moat_periodic_is_active(in_this->periodic);

  if (!is_active) {
    err = moat_periodic_start(in_this->periodic);
    if (err != SSE_E_OK) {
      SSE_LOG_ERROR(TAG, "moat_periodic_start() ... failed.");
      return err;
    }
  }
  return SSE_E_OK;
}

sse_int
Uploader_stop(Uploader *in_this)
{
  sse_bool is_active;

  is_active = moat_periodic_is_active(in_this->periodic);

  if (is_active) {
    moat_periodic_stop(in_this->periodic);
  }
  return SSE_E_OK;
}

sse_int
Uploader_post(Uploader *in_this, MoatObject* in_object)
{
  sse_int err;

  err = sse_queue_enqueue(in_this->queue, in_object);
  if (err != SSE_E_OK) {
    SSE_LOG_ERROR(TAG, "sse_queue_enqueue() ... failed with [%d].", err);
    return err;
  }
  return SSE_E_OK;
}
