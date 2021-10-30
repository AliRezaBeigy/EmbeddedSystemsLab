#define F_CPU 1000000UL /* Define CPU Frequency e.g. here 8MHz */

#include <avr/io.h>
#include <avr/interrupt.h>

int t = 0;
unsigned int counter = 0;
char T[] = {0b0101, 0b0110, 0b1010, 0b1001};

ISR(TIMER0_OVF_vect)
{
    counter++;
    TCNT0 = 130;

    if (counter == 5)
    {
        counter = 0;

        if ((PINC & (1 << PIN2)) == 0)
        {
            PORTA = T[t];
            t++;
            if (t == 4)
            {
                t = 0;
            }
        }
    }
}

int main()
{
    DDRC = 0;
    DDRA = 0xFF;

    PORTA = 0x00;

    TIMSK = 1 << TOIE0; // Enable Timer0
    TCCR0 |= 1 << CS01; // Clk/8 Prescaler

    sei();

    while (true)
    {
    }
}
