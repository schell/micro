//
//  pin.c
//
//  Created by Schell Scivally on 1/28/15.
//  Copyright (c) 2015 zyghost. All rights reserved.
//

#include "pin.h"

pin pin_make(volatile uint8_t *port, volatile uint8_t *ddr, volatile uint8_t *pinr, uint8_t bit) {
    pin p;
    p.port = port;
    p.ddr = ddr;
    p.pinr = pinr;
    p.bit = bit;
    return p;
}

void set_pin(pin p) {
    set_bit(*p.port, p.bit);
}

void clear_pin(pin p) {
    clear_bit(*p.port, p.bit);
}

bool read_pin(pin p) {
    return read_bit(*p.pinr, p.bit) ? 1 : 0;
}

void output_pin(pin p) {
    set_bit(*p.ddr, p.bit);
}

void input_pin(pin p) {
    clear_bit(*p.ddr, p.bit);
}