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

#ifndef __SENSOR_H__
#define __SENSOR_H__

#ifdef __cplusplus
extern "C" {
#endif

struct _Sensor {
  MoatTimer* timer;
  sse_int timer_id;
  sse_int interval;
  sse_char * event_file;
  SSEList* listeners;
};
typedef struct _Sensor Sensor;

typedef sse_int (*SensorListenerCallback)(sse_int32 in_temperature, sse_pointer in_user_data);

struct _SensorListener {
  sse_pointer user_data;
  SensorListenerCallback callback;
};
typedef struct _SensorListener SensorListener;

Sensor* Sensor_new();
sse_int Sensor_free(Sensor* in_this);
sse_int Sensor_subscribe(Sensor* in_this, SensorListener* in_listener);
sse_int Sensor_unsubscribe(Sensor* in_this, SensorListener* in_listener);
void Sensor_set_interval(Sensor* in_this, sse_int in_interval);
sse_int Sendor_get_interval(Sensor* in_this);

SensorListener* SensorListener_new(sse_pointer user_data, SensorListenerCallback callback);
void SensorListener_free(SensorListener* in_this);

#ifdef __cplusplus
}
#endif

#endif /* __SENSOR_H__ */
