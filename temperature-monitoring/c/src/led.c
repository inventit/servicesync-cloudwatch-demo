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
#include "led.h"

#define TAG "Sample:LED"

sse_char *g_led_path[4];

static sse_int led_set_brightness(const sse_char *led, const sse_char *brightness)
{
  FILE* fd;

  if ((fd = fopen(led, "w")) == NULL) {
    int err_no = errno;
    SSE_LOG_ERROR(TAG, "foepn() ... failed with %s", strerror(err_no));
    return SSE_E_GENERIC;
  }

  fputs(brightness, fd);
  fclose(fd);

  return SSE_E_OK;
}

void
LED_init(void)
{
  sse_int i;
  sse_char *prefix;

  prefix = getenv("SSEGW_SAMPLE_LED_PREFIX");
  if (prefix == NULL) {
    prefix = "/sys/class/leds";
  }

  for (i = 0; i < 4; i++) {
    sse_char buff[256];
    sprintf(buff, "%s/led%d/brightness", prefix, i + 1);
    g_led_path[i] = strdup(buff);
  }
}

void
LED_teardown(void)
{
  sse_int i;

  for (i = 0; i < 4; i++) {
    if (g_led_path[i] != NULL) {
      sse_free(g_led_path[i]);
    }
  }
}

void
LED_turn_on(LED in_led)
{
  SSE_LOG_INFO(TAG, "Turn on: %s", g_led_path[in_led]);
  led_set_brightness(g_led_path[in_led], "255");
}

void
LED_turn_off(LED in_led)
{
  SSE_LOG_INFO(TAG, "Turn off: %s", g_led_path[in_led]);
  led_set_brightness(g_led_path[in_led], "0");
}

