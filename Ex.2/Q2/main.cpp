#define F_CPU 1000000UL /* Define CPU Frequency 1MHz */

#include <avr/io.h>
#include <avr/delay.h>

int main()
{
    DDRA = 0;
    DDRB = 0xFF;

    while (true)
    {
        if((PINA & 1) == 0) {
            PORTB ^= 0xFF;
            _delay_ms(500);
        }
    }
    return 0;
}
