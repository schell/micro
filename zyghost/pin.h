//
//  pin.h
//
//  Created by Schell Scivally on 1/28/15.
//  Copyright (c) 2015 zyghost. All rights reserved.
//

#ifndef lcd_pin_h
#define lcd_pin_h

#include <avr/io.h>
#include <avr/pgmspace.h>

#define set_bit(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#define clear_bit(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define read_bit(sfr, bit) (_SFR_BYTE(sfr) & _BV(bit)) 
#define OUTPUT 1
#define INPUT 0
#define HIGH 1
#define LOW 0

typedef enum {
    true = 1,
    false = 0
} bool;

typedef struct {
    volatile uint8_t *port;
    volatile uint8_t *ddr;
    volatile uint8_t *pinr;
    uint8_t bit;
} pin;

pin pin_make(volatile uint8_t *port, volatile uint8_t *ddr, volatile uint8_t *pinr, uint8_t bit);
void output_pin(pin p);
void set_pin(pin p);
void clear_pin(pin p);
void input_pin(pin p);
bool read_pin(pin p);

#endif
