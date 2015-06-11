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

#ifndef __UPLOADER_H__
#define __UPLOADER_H__

#ifdef __cplusplus
extern "C" {
#endif

struct _Uploader {
  Moat moat;
  sse_char* model_name;
  sse_uint interval;
  MoatPeriodic* periodic;
  SSEQueue* queue;
};
typedef struct _Uploader Uploader;

static const sse_uint UploaderDefaultInterval = 60;

Uploader* Uploader_new(Moat in_moat, const sse_char* in_model_name);
void Uploader_free(Uploader *in_this);
sse_int Uploader_start(Uploader *in_this);
sse_int Uploader_stop(Uploader *in_this);
sse_int Uploader_post(Uploader *in_this, MoatObject* in_object);

#ifdef __cplusplus
}
#endif

#endif /* __UPLOADER_H__ */
