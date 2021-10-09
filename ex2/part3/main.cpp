#define F_CPU 1000000UL /* Define CPU Frequency 1MHz */

#include <avr/io.h>
#include <avr/delay.h>

int main()
{
    DDRA = 0;
    DDRB = 0xFF;

    bool direction = 1;
    
    while (true)
    {
        if ((PINA & 1) == 0)
        {
            // Initialization
            if(PORTB == 0){
                PORTB = 1;
                _delay_ms(500);
            }

            // Shift blinking LED
            if (direction)
                PORTB <<= 1;
            else
                PORTB >>= 1;

            // Chnage direction when first and last LED blinked
            if (PORTB == 0)
                direction = 1;
            if (PORTB & (1 << PORTB7))
                direction = 0;

            _delay_ms(500);
        }
    }
    return 0;
}
