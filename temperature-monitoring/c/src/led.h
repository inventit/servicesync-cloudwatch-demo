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

#ifndef __LED_H__
#define __LED_H__

#ifdef __cplusplus
extern "C" {
#endif

enum LED_ {
    LED1,
    LED2,
    LED3,
    LED4,
    LEDs
};
typedef enum LED_ LED;

void LED_init(void);
void LED_teardown(void);
void LED_turn_on(LED led);
void LED_turn_off(LED led);

#ifdef __cplusplus
}
#endif

#endif /* __LED_H__ */
