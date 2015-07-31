//
//  main.c
//  test
//
//  Created by Schell Scivally on 1/22/15.
//  Copyright (c) 2015 zyghost. All rights reserved.
//

#include <stdio.h>
#include <string.h>

// Flips a flag at a bit position either on or off.
void flip_bit(uint8_t* val, int bit, int on) {
    if (on) {
        *val |= 1 << bit;
    } else {
        *val &= ~(1 << bit);
    }
}

int main(int argc, const char * argv[]) {
    char* str = "";
    if (strcmp(str, "") == 0) {
        printf("Equality!\n");
    }
    printf("Hello, World!\n%i %i\n",0b11111110, 0xFE);
    return 0;
}
