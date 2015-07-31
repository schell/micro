#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <string.h>
#include "zyghost/zyghost.h"
#include "zyghost/lcd.h"
#include "zyghost/encoder.h"

typedef struct {
    uint16_t clock;
    uint16_t millis;
    uint16_t seconds;
    uint16_t hours;
} time;

#define TIME_ZERO time_make(0,0,0,0)

bool time_is_greater(time a, time b) {
    if (a.hours > b.hours) {
        return true;
    }
    if (a.hours < b.hours) {
        return false;
    }
   
    if (a.seconds > b.seconds) {
        return true;
    }
    if (a.seconds < b.seconds) {
        return false;
    }
    
    if (a.millis > b.millis) {
        return true;
    }
    if (a.millis < b.millis) {
        return false;
    }
    
    if (a.clock > b.clock) {
        return true;
    }
    return false;
}

time time_update(time t, uint16_t clks) {
    t.clock += clks;
    if (t.clock >= (F_CPU / 1000)) {
        t.millis++;
        t.clock -= (F_CPU / 1000);
        if (t.millis >= 1000) {
            t.millis -= 1000;
            t.seconds++;
            if (t.seconds >= 3600) {
                t.seconds -= 3600;
                t.hours++;
            }
        }
    }
    return t;
}

time time_make(uint16_t clk, uint16_t ms, uint16_t s, uint16_t h) {
    time t;
    t.clock = clk;
    t.millis = ms;
    t.seconds = s;
    t.hours = h;
    return time_update(t, 0);
}

time time_add(time a, time b) {
    time aa = time_update(a, 0);
    time bb = time_update(b, 0);
    return time_make(aa.clock + bb.clock, aa.millis + bb.millis, aa.seconds + bb.seconds, aa.hours + bb.hours);
}

time time_diff_internal(time a, time b) {
    if (time_is_greater(b, a)) {
        return TIME_ZERO;
    }
    
    uint16_t clocka = a.clock;
    uint16_t millia = a.millis;
    uint16_t seconda = a.seconds;
    uint16_t hoursa = a.hours;
    
    uint16_t clockb = b.clock;
    uint16_t millib = b.millis;
    uint16_t secondb = b.seconds;
    uint16_t hoursb = b.hours;
    
    while (clocka < clockb && millia > 0) {
        millia--;
        clocka += 256;
        if (millia == 0) {
            seconda--;
            millia += 1000;
            if (seconda == 0) {
                hoursa--;
                seconda += 3600;
            }
        }
    }
    
    while (millia < millib && seconda > 0) {
        seconda--;
        millia += 1000;
        if (seconda == 0) {
            hoursa--;
            seconda += 3600;
        }
    }
    
    while (seconda < secondb && hoursa > 0) {
        hoursa--;
        seconda += 3600;
    }
    
    time t;
    t.clock = clocka - clockb;
    t.millis = millia - millib;
    t.seconds = seconda - secondb;
    t.hours = hoursa - hoursb;
    return time_update(t, 0);
}

time time_diff(time a, time b) {
    if (time_is_greater(b, a)) {
        return time_diff_internal(b, a);
    }
    
    return time_diff_internal(a, b);
}

void time_str(char* str, time t) {
    sprintf(str, "%uh %us %ums", t.hours, t.seconds, t.millis);
}

volatile time timer;
ISR(TIMER0_OVF_vect) {
    // Using 8bit timer we've traversed 256 clock cycles
    timer = time_update(timer, 256);
}

void timer_start(void) {
    // Setup our timer interrupt
    TCCR0A = 0;
    TCCR0B = 0;
    TCNT0 = 0;
    TIMSK0 = _BV(TOIE0);
    timer.clock = 0;
    timer.millis = 0;
    timer.seconds = 0;
    timer.hours = 0;
    // Start it!
    TCCR0B = 1;
}

void timer_stop(void) {
    TCCR0A = 0;
    TCCR0B = 0;
    TCNT0 = 0;
    TIMSK0 = _BV(TOIE0);
    timer.clock = 0;
    timer.millis = 0;
    timer.seconds = 0;
    timer.hours = 0;
}

void time_set_global(time* t) {
    t->clock = timer.clock;
    t->millis = timer.millis;
    t->seconds = timer.seconds;
    t->hours = timer.hours;
}

void lcd_clear(hd44780* lcd, char* str) {
    uint8_t flags = lcd_display_clear_flags();
    lcd_send_command(lcd, flags);
    flags = lcd_return_home_flags();
    lcd_send_command(lcd, flags);
    if (strcmp(str, "") != 0) {
        lcd_write(lcd, str);
    }
}

typedef enum {
    screen_unknown,
    screen_startup,
    screen_menu,
    screen_set_drain
} screen;

typedef struct {
    pin h2o_sensor;
    pin pump_relay;
    hd44780 lcd;
    encoder enc;
    screen screen;
    time when_drain_ends;
    time next_h2o_check;
    bool draining;
    uint16_t drain_seconds;
    uint16_t h2o_threshold;
} foodunit;

void foodunit_startup(foodunit* f);
void foodunit_update_current_screen(foodunit* f);
void foodunit_display(foodunit* f, screen s);
void foodunit_init(foodunit* f);
void foodunit_drain(foodunit* f);
void foodunit_update_h2o_check(foodunit* f);
void foodunit_fill(foodunit* f);
void foodunit_update(foodunit* f);

void foodunit_update_current_screen(foodunit* f) {
    // This is where we set up the functionality for our current screen.
    switch (f->screen) {
        default:
            break;
    }
}

int8_t foodunit_read_user(foodunit* f, uint8_t x, uint8_t y, const char* suffix) {
    f->enc.steps = 0;
    char str[20];
    int8_t steps = f->enc.steps;
    while (!encoder_btn_up_edge(&f->enc)) {
        encoder_update(&f->enc);
        if (steps != f->enc.steps) {
            lcd_xy(&f->lcd, x, y);
            sprintf(str, "%3i",f->enc.steps);
            lcd_write(&f->lcd, str);
            steps = f->enc.steps;
        }
    }
    return 0;
}

void foodunit_display(foodunit* f, screen s) {
    // We've already displayed the current screen, all we may need to do is
    // update it.
    if (f->screen == s) {
        foodunit_update_current_screen(f);
        return;
    }
    
    // This is where we do the initial screen setup.
    switch (s) {
        case screen_startup: {
            lcd_clear(&f->lcd, "FoodUnit v0.0\nPlease push the\nencoder to start.\n>Ok");
            lcd_xy(&f->lcd, 0, 3);
            while (!encoder_btn_up_edge(&f->enc)) {
                encoder_update(&f->enc);
            }
            encoder_clear(&f->enc);
            foodunit_display(f, screen_menu);
            break;
        }
            
        case screen_menu: {
            lcd_clear(&f->lcd, "Please set the drain\nduration.");
            foodunit_read_user(f, 0, 3, "minutes");
            foodunit_display(f, screen_unknown);
            break;
        }
            
        default:
            lcd_clear(&f->lcd, "wtf");
            break;
    }
    
    f->screen = s;
}

void foodunit_init(foodunit* f) {
    // Analog h2o pin F0
    analogReference(ADC_REF_POWER);
    f->h2o_sensor = pin_make(&PORTF, &DDRF, &PINF, 0);
    input_pin(f->h2o_sensor);
    
    // The pump relay.
    f->pump_relay = pin_make(&PORTB, &DDRB, &PINB, 6);
    output_pin(f->pump_relay);
    clear_pin(f->pump_relay);
    
    // Initial programmable values.
    f->draining = true;
    f->when_drain_ends = time_make(0, 0, 0, 0);
    f->next_h2o_check = time_make(0, 0, 0, 0);
    f->drain_seconds = 10;
    f->h2o_threshold = 50;
    f->screen = screen_unknown;
    
    // The lcd screen.
    f->lcd.rs = pin_make(&PORTD, &DDRD, &PIND, 5);
    f->lcd.rw = pin_make(&PORTD, &DDRD, &PIND, 6);
    f->lcd.e = pin_make(&PORTD, &DDRD, &PIND, 7);
    for (int i = 0; i < 8; i++) {
        f->lcd.db[i] = pin_make(&PORTC, &DDRC, &PINC, i);
    }
    f->lcd.mode = LCD_8BITS;
    lcd_init(&f->lcd);
    lcd_send_command(&f->lcd, lcd_display_toggle_flags(LCD_DISPLAYON, LCD_CURSORON, LCD_CURSORBLINKOFF));
    
    // The rotary encoder/pushbutton
    encoder_init(&f->enc);
    f->enc.btn = pin_make(&PORTD, &DDRD, &PIND, 1);
    set_pin(f->enc.btn); // pull up
    f->enc.a = pin_make(&PORTD, &DDRD, &PIND, 2);
    f->enc.b = pin_make(&PORTD, &DDRD, &PIND, 3);
    
    // Show our startup screen.
    foodunit_display(f, screen_startup);
}

/** Read the water sensor. */
uint16_t water_level(pin* h2o) {
    int avg = 0;
    for (int i = 0; i < 10; i++) {
        uint16_t adc = adc_read(h2o->bit);
        avg += adc;
    }
    return avg / 10;
}

void foodunit_drain(foodunit* f) {
    f->draining = true;
    clear_pin(f->pump_relay);
    time to_drain = time_make(0, 0, f->drain_seconds, 0);
    f->when_drain_ends = time_add(timer, to_drain);
}

void foodunit_update_h2o_check(foodunit* f) {
    f->next_h2o_check = time_add(timer, time_make(0, 100, 0, 0));
}

void foodunit_fill(foodunit* f) {
    f->draining = false;
    set_pin(f->pump_relay);
    foodunit_update_h2o_check(f);
}

void foodunit_update(foodunit* f) {
    encoder_update(&f->enc);
    foodunit_display(f, f->screen);
    
    if (f->draining) {
        if (time_is_greater(timer, f->when_drain_ends)) {
            foodunit_fill(f);
        } else {
            // Update the drain display
            time t = time_diff(timer, f->when_drain_ends);
            char t1[16];
            time_str(t1, t);
            lcd_xy(&f->lcd, 0, 1);
            lcd_write(&f->lcd, t1);
        }
    } else {
        if (time_is_greater(timer, f->next_h2o_check)){
            foodunit_update_h2o_check(f);
            uint16_t lvl = water_level(&f->h2o_sensor);
            if (lvl < f->h2o_threshold) {
                foodunit_drain(f);
            }
        }
    }
    
}

int main(void) {
    CPU_PRESCALE(CPU_16MHz);
    usb_init();
    while (!usb_configured()) {
        continue;
    }
    timer = time_make(0, 0, 0, 0);
    timer_start();
    
    foodunit f;
    foodunit_init(&f);
    while (1) {
        foodunit_update(&f);
    }
}