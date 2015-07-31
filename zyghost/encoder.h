//
//  encoder.h
//  lcd
//
//  Created by Schell Scivally on 2/6/15.
//  Copyright (c) 2015 zyghost. All rights reserved.
//

#ifndef __encoder__
#define __encoder__

#include <stdio.h>
#include "zyghost.h"
#include "pin.h"

typedef struct {
    pin a;
    pin b;
    pin btn;
    uint8_t val;
    uint8_t btn_val;
    int8_t steps;
} encoder;

void encoder_init(encoder* enc);
void encoder_clear(encoder* enc);
int8_t encoder_get_step(encoder* enc);
void encoder_update(encoder* enc);
void encoder_update(encoder* e);
bool encoder_btn_down(encoder* e);
bool encoder_btn_up(encoder* e);
bool encoder_btn_down_edge(encoder* e);
bool encoder_btn_up_edge(encoder* e);

#endif /* defined(__encoder__) */
