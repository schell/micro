//
//  lcd.c
//
//  Created by Schell Scivally on 1/28/15.
//  Copyright (c) 2015 zyghost. All rights reserved.
//

#include <avr/pgmspace.h>
#include <util/delay.h>
#include "lcd.h"


// Flips a flag at a bit position either on or off.
void flip_bit(uint8_t* val, uint8_t bit, bool high) {
    if (high) {
        *val |= _BV(bit);
    } else {
        *val &= ~_BV(bit);
    }
}

/** Constructs flags for interface data length (dl), number of display lines (n) 
 * and character font (f).
 
 DL: Sets the interface data length. Data is sent or received in 8-bit lengths 
     (DB7 to DB0) when DL is 1, and in 4-bit lengths (DB7 to DB4) when DL is 0.
     When 4-bit length is selected, data must be sent or received twice.
 DL = 1: 8 bits,      DL = 0: 4 bits
 
 N: Sets the number of display lines.
 N  = 1: 2 lines,     N  = 0: 1 line
 
 F: Sets the character font.
 F  = 1: 5 × 10 dots, F  = 0: 5 × 8 dots
 
 Note: Perform the function at the head of the program before executing any 
 instructions (except for the read busy flag and address instruction). From this 
 point, the function set instruction cannot be executed unless the interface 
 data length is changed.
 */
uint8_t lcd_function_set_flags(bool dl, bool n, bool f) {
    uint8_t flags = 0;
    flip_bit(&flags, 5, 1);
    flip_bit(&flags, 4, dl);
    flip_bit(&flags, 3, n);
    flip_bit(&flags, 2, f);
    return flags;
}

/** Constructs flags for display on/off control.
 D: The display is on when D is 1 and off when D is 0. When off, the display 
    data remains in DDRAM, but can be displayed instantly by setting D to 1.
 C: The cursor is displayed when C is 1 and not displayed when C is 0. Even if 
    the cursor disappears, the function of I/D or other specifications will not
    change during display data write. The cursor is displayed using 5 dots in 
    the 8th line for 5 × 8 dot character font selection and in the 11th line for
    the 5 × 10 dot character font selection (Figure 13).
 B: The character indicated by the cursor blinks when B is 1 (Figure 13). The 
    blinking is displayed as switching between all blank dots and displayed 
    characters at a speed of 409.6-ms intervals when fcp or fOSC is 250 kHz. 
    The cursor and blinking can be set to display simultaneously. (The blinking 
    frequency changes according to fOSC or the reciprocal of fcp. For example, 
    when fcp is 270 kHz, 409.6 × 250/270 = 379.2 ms.)
 */
uint8_t lcd_display_toggle_flags(bool d, bool c, bool b) {
    uint8_t flags = 0;
    flip_bit(&flags, 3, 1);
    flip_bit(&flags, 2, d);
    flip_bit(&flags, 1, c);
    flip_bit(&flags, 0, b);
    return flags;
}

/** Constructs flags for clearing the display.
 
 Clear display writes space code 20H (character pattern for character code 20H 
 must be a blank pattern) into all DDRAM addresses. It then sets DDRAM address 0 
 into the address counter, and returns the display to its original status if it 
 was shifted. In other words, the display disappears and the cursor or blinking 
 goes to the left edge of the display (in the first line if 2 lines are 
 displayed). It also sets I/D to 1 (increment mode) in entry mode. S of entry 
 mode does not change.
 */
uint8_t lcd_display_clear_flags(void) {
    return 1;
}

/** Constructs flags for setting the entry mode.
 
 I/D: Increments (I/D = 1) or decrements (I/D = 0) the DDRAM address by 1 when a 
      character code is written into or read from DDRAM. The cursor or blinking 
      moves to the right when incremented by 1 and to the left when decremented 
      by 1. The same applies to writing and reading of CGRAM.
 S: Shifts the entire display either to the right (I/D = 0) or to the left 
    (I/D = 1) when S is 1. The display does not shift if S is 0.
    If S is 1, it will seem as if the cursor does not move but the display does. 
    The display does not shift when reading from DDRAM. Also, writing into or 
    reading out from CGRAM does not shift the display.
 */
uint8_t lcd_entry_mode_set_flags(bool id, bool s) {
    uint8_t flags = 0;
    flip_bit(&flags, 2, 1);
    flip_bit(&flags, 1, id);
    flip_bit(&flags, 0, s);
    return flags;
}

uint8_t lcd_return_home_flags(void) {
    uint8_t flags = 0;
    flip_bit(&flags, 1, 1);
    return flags;
}

/**
 Set DDRAM address sets the DDRAM address binary AAAAAAA into the address counter.
 Data is then written to or read from the MPU for DDRAM.
 However, when N is 0 (1-line display), AAAAAAA can be 00H to 4FH. When N is 1 (2-line display),
 AAAAAAA can be 00H to 27H for the first line, and 40H to 67H for the second line.
 */
uint8_t lcd_set_ddram_addr(uint8_t address) {
    uint8_t flags = 0x80 + address;
    return flags;
}

/** Selects between the instruction (command) and data registers on the given
 lcd.
 */
void lcd_select_register(hd44780 *lcd, bool selected_register) {
    if (selected_register == LCD_COMMAND_REGISTER) {
        //clear_bit(PORTD, 7);
        clear_pin(lcd->rs);
    } else {
        //set_bit(PORTD, 7);
        set_pin(lcd->rs);
    }
}

void lcd_toggle_send_register(hd44780* lcd, bool lcd_register) {
    // Set the E wire
    set_pin(lcd->e);
    // Select register
    lcd_select_register(lcd, lcd_register);
    // Set RW to write
    clear_pin(lcd->rw);
    // Clear E
    clear_pin(lcd->e);
    // Wait a bit
    _delay_ms(3);
}

void pin_set_bit(pin p, bool high) {
    if (high) {
        set_pin(p);
    } else {
        clear_pin(p);
    }
}

void lcd_send_nibble8(hd44780 *lcd, char nibble, bool lcd_register) {
    // Put the nibble (or byte) on the port lines.
    //_SFR_BYTE(*lcd->db[0].port) = nibble;
    for (int i = 0; i < 8; i++) {
        bool high = (nibble >> i) & 0x01;
        if (high) {
            set_pin(lcd->db[i]);
        } else {
            clear_pin(lcd->db[i]);
        }
    }
    
    lcd_toggle_send_register(lcd, lcd_register);
}

void lcd_send_nibble4(hd44780* lcd, char nibble, bool lcd_register) {
    for (int i = 0; i < 4; i++) {
        bool high = (nibble >> i) & 0x01;
        if (high) {
            set_pin(lcd->db[i + 4]);
        } else {
            clear_pin(lcd->db[i + 4]);
        }
    }

    lcd_toggle_send_register(lcd, lcd_register);
}

void lcd_send_byte(hd44780 *lcd, char byte, bool lcd_register) {
    if (lcd->mode == LCD_4BITS) {
        lcd_send_nibble4(lcd, byte >> 4, lcd_register);
        lcd_send_nibble4(lcd, byte, lcd_register);
    } else {
        lcd_send_nibble8(lcd, byte, lcd_register);
    }
}

void lcd_send_data(hd44780 *lcd, char byte) {
    lcd_send_byte(lcd, byte, LCD_DATA_REGISTER);
}

void lcd_send_command(hd44780* lcd, char byte) {
    lcd_send_byte(lcd, byte, LCD_COMMAND_REGISTER);
}

void lcd_init(hd44780* lcd) {
    // Set the pin outputs just in case that hasn't happend yet.
    output_pin(lcd->rs);
    output_pin(lcd->rw);
    output_pin(lcd->e);
    for (int i = 0; i < 8; i++) {
        if (lcd->mode == LCD_4BITS && i < 4) {
            continue;
        }
        output_pin(lcd->db[i]);
    }
    
    _delay_ms(20);				    // display reset takes 20ms
    
    uint8_t op = lcd_function_set_flags(LCD_8BITS, 0, 0);
    lcd_send_nibble8(lcd, op, LCD_COMMAND_REGISTER);
    
    _delay_ms(5);
    
    lcd_send_nibble8(lcd, op, LCD_COMMAND_REGISTER);
    
    _delay_ms(1);

    lcd_send_nibble8(lcd, op, LCD_COMMAND_REGISTER);
    
    _delay_ms(1);
    
    // set interface to mode
    uint8_t funset = lcd_function_set_flags(lcd->mode, 0, 0);
    lcd_send_nibble8(lcd, funset, LCD_COMMAND_REGISTER);
    
    // Basic initialization is done and we can start talking in our target mode.
    // Set display lines and font
    funset = lcd_function_set_flags(lcd->mode, LCD_2LINES, LCD_5x8DOTS);
    lcd_send_command(lcd, funset);
    
    // Set display, cursor and cursor blinking
    uint8_t display = lcd_display_toggle_flags(LCD_DISPLAYON, LCD_CURSORON, LCD_CURSORBLINKON);
    lcd_send_command(lcd, display);
    
    // Clear it
    lcd_send_command(lcd, lcd_display_clear_flags());
    
    // Set increment and shifting
    uint8_t entry = lcd_entry_mode_set_flags(LCD_ENTRYINC, LCD_ENTRYSHIFTOFF);
    lcd_send_command(lcd, entry);
}

static const uint8_t __line_address[] = {0, 0x40, 0x14, 0x54};

/** Writes the given string to the lcd. */
void lcd_write(hd44780 *lcd, const char *str) {
    uint8_t line = 0;
    while (*str) {
        char c = *str++;
        if (c == '\n') {
            line += 1;
            uint8_t addr = __line_address[line % 4];
            uint8_t flags = lcd_set_ddram_addr(addr);
            lcd_send_command(lcd, flags);
            continue;
        }
        lcd_send_data(lcd, c);
    }
}

void lcd_xy(hd44780 *lcd, uint8_t x, uint8_t y) {
    uint8_t addr = lcd_set_ddram_addr(__line_address[y % 4] + x);
    lcd_send_command(lcd, addr);
}