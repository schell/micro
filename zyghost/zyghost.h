//
//  zyghost.h
//  FoodUnit
//
//  Created by Schell Scivally on 2/17/15.
//  Copyright (c) 2015 zyghost. All rights reserved.
//

#ifndef _zyghost_h_
#define _zyghost_h_

// CPU Stuff
#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))
#define CPU_16MHz       0x00
#define CPU_8MHz        0x01
#define CPU_4MHz        0x02
#define CPU_2MHz        0x03
#define CPU_1MHz        0x04
#define CPU_500kHz      0x05
#define CPU_250kHz      0x06
#define CPU_125kHz      0x07
#define CPU_62kHz       0x08

#define LED_ON      (PORTD |= (1<<6))
#define LED_OFF		(PORTD &= (~(1<<6)))

#include "pin.h"
#include "analog.h"
#include "usb_debug_only.h"

// Helpers
void byte_to_binary(char* b, uint8_t x);
uint8_t zstrlen(char* s);
#endif
