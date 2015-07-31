//
//  lcd.h
//  lcd
//  For help @see https://en.wikipedia.org/wiki/Hitachi_HD44780_LCD_controller
//
//  Created by Schell Scivally on 1/21/15.
//  Copyright (c) 2015 zyghost. All rights reserved.
//

#ifndef lcd_h
#define lcd_h

#include <avr/io.h>
#include "pin.h"

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
#define LCD_8BITS 1
#define LCD_4BITS 0
#define LCD_2LINES 1
#define LCD_1LINE 0
#define LCD_5x10DOTS 1
#define LCD_5x8DOTS 0
uint8_t lcd_function_set_flags(bool dl, bool n, bool f);

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
#define LCD_DISPLAYON 1
#define LCD_DISPLAYOFF 0
#define LCD_CURSORON 1
#define LCD_CURSOROFF 0
#define LCD_CURSORBLINKON 1
#define LCD_CURSORBLINKOFF 0
uint8_t lcd_display_toggle_flags(bool d, bool c, bool b);

/** Constructs flags for clearing the display.
 
 Clear display writes space code 20H (character pattern for character code 20H 
 must be a blank pattern) into all DDRAM addresses. It then sets DDRAM address 0 
 into the address counter, and returns the display to its original status if it 
 was shifted. In other words, the display disappears and the cursor or blinking 
 goes to the left edge of the display (in the first line if 2 lines are 
 displayed). It also sets I/D to 1 (increment mode) in entry mode. S of entry 
 mode does not change.
 */
uint8_t lcd_display_clear_flags(void);

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
#define LCD_ENTRYINC 1
#define LCD_ENTRYDEC 0
#define LCD_ENTRYSHIFTON 1
#define LCD_ENTRYSHIFTOFF 0
uint8_t lcd_entry_mode_set_flags(bool id, bool s);

/** */
uint8_t lcd_return_home_flags(void);

/**
 Set DDRAM address sets the DDRAM address binary AAAAAAA into the address counter.
 Data is then written to or read from the MPU for DDRAM.
 However, when N is 0 (1-line display), AAAAAAA can be 00H to 4FH. When N is 1 (2-line display),
 AAAAAAA can be 00H to 27H for the first line, and 40H to 67H for the second line.
 */
uint8_t lcd_set_ddram_addr(uint8_t address);

/** Our hd44780 lcd structure. 
 * TODO: Maybe these members should be pin*'s to save space? 
 */
typedef struct {
    pin rs;     // Register select
    pin rw;     // Read/Write
    pin e;      // Enable
    pin db[8];  // Data bus in 4 bit mode we will only use db[4-7]
    uint8_t mode;
} hd44780;

/** Selects between the instruction (command) and data registers on the given
 lcd.
 */
#define LCD_COMMAND_REGISTER 0
#define LCD_DATA_REGISTER 1
void lcd_select_register(hd44780 *lcd, bool selected_register);

/** */
void lcd_send_nibble8(hd44780 *lcd, char nibble, bool lcd_register);
void lcd_send_nibble4(hd44780 *lcd, char nibble, bool lcd_register);

/** */
void lcd_send_byte(hd44780 *lcd, char byte, bool lcd_register);

/** */
void lcd_send_data(hd44780 *lcd, char byte);

/** */
void lcd_send_command(hd44780 *lcd, char byte);

/** */
void lcd_init(hd44780 *lcd);

/** */
void lcd_write(hd44780 *lcd, const char *str);

/** Sets the cursor to a given x,y character. */
void lcd_xy(hd44780 *lcd, uint8_t x, uint8_t y);

#endif
