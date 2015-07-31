#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))
#define LED_ON      (PORTD |= (1<<6))
#define LED_OFF		(PORTD &= (~(1<<6)))

int main(void) {
    DDRD |= _BV(6);
    PORTD |= _BV(6);

	while (1) {
	}
	return 0; // never reached
}
