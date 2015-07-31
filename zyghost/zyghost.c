//
//  zyghost.c
//  FoodUnit
//
//  Created by Schell Scivally on 2/22/15.
//  Copyright (c) 2015 zyghost. All rights reserved.
//

#include <stdio.h>

void byte_to_binary(char* b, uint8_t x) {
    for (int i = 7; i >= 0; i--) {
        b[i] = x >> i & 0x01 ? '1' : '0';
    }
    b[8] = '\0';
}

uint8_t zstrlen(char* s) {
    uint8_t i = 0;
    while (i < 255 && s[i] != '\0') {
        i++;
    }
    return i;
}