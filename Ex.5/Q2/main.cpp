#include <avr/io.h>
#include <avr/interrupt.h>

int t = 0;
unsigned int counter = 0;
char T[] = {0b0101, 0b0110, 0b1010, 0b1001};
int mode = 0; // Clockwise(0), AntiClockwise(1)

ISR(TIMER0_OVF_vect)
{
    counter++;
    TCNT0 = 130;

    if (counter == 5)
    {
        counter = 0;
        PORTA = T[t];

        if (mode == 0)
        {
            t++;
        }
        else
        {
            t--;
        }
        if (t == 4)
        {
            t = 0;
        }
        if (t == -1)
        {
            t = 3;
        }
    }
}

ISR(INT0_vect)
{
    mode ^= 1;
}

int main()
{
    DDRA = 0xFF;

    PORTA = 0x00;

    TIMSK = 1 << TOIE0;              // Enable Timer0
    TCCR0 |= 1 << CS01;              // Clk/8 Prescaler
    GICR = 1 << INT0;                // Enable INT0
    MCUCR = 1 << ISC01 | 1 << ISC00; // Trigger INT0 on raising edge

    sei();

    while (true)
    {
    }
}
