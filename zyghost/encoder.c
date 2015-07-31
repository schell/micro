//
//  encoder.c
//  lcd
//
//  Created by Schell Scivally on 2/6/15.
//  Copyright (c) 2015 zyghost. All rights reserved.
//

#include "encoder.h"

static const int8_t enc_states[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
static const uint8_t enc_edge_up = 0x01;
static const uint8_t enc_edge_down = 0xFE;
static const uint8_t enc_up = 0xFF;

void encoder_init(encoder* enc) {
    // Set btn to read a pushbutton
    input_pin(enc->btn);
    set_pin(enc->btn);
    
    // Set a and b to the rotary encoder gray code inputs
    // Use the tutorial to read gray code
    // @see http://www.circuitsathome.com/mcu/programming/reading-rotary-encoder-on-arduino
    input_pin(enc->a);
    set_pin(enc->a);
    input_pin(enc->b);
    set_pin(enc->b);
    
    encoder_clear(enc);
    
}

void encoder_clear(encoder* enc) {
    enc->val = 0;
    enc->steps = 0;
    enc->btn_val = enc_up;
}

int8_t encoder_get_step(encoder* enc) {
    // Handle the rotary gray code.
    uint8_t a = read_pin(enc->a);
    uint8_t b = read_pin(enc->b);
    
    enc->val <<= 2;
    enc->val |= (a << 1) | b;
    
    return enc_states[enc->val & 0x0f];
}

void encoder_update(encoder* e) {
    e->steps += encoder_get_step(e);
    e->btn_val <<= 1;
    e->btn_val |= read_pin(e->btn);
}

bool encoder_btn_down(encoder* e) {
    return e->btn_val & 0x01;
}

bool encoder_btn_up(encoder* e) {
    return !encoder_btn_down(e);
}

bool encoder_btn_down_edge(encoder* e) {
    return (e->btn_val == enc_edge_down);
}

bool encoder_btn_up_edge(encoder* e) {
    return (e->btn_val == enc_edge_up);
}