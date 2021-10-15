#include <avr/io.h>
#include <avr/interrupt.h>

int counter = 0;

int numTo7Seg(int num)
{
    switch (num)
    {
        case 0:
            return 0x3F;
        case 1:
            return 0x06;
        case 2:
            return 0x5B;
        case 3:
            return 0x4F;
        case 4:
            return 0x66;
        case 5:
            return 0x6D;
        case 6:
            return 0x7D;
        case 7:
            return 0x07;
        case 8:
            return 0x7F;
        case 9:
            return 0xEF;
    }
    return 0;
}

ISR(TIMER1_OVF_vect)
{
    PORTD = numTo7Seg(counter % 10);
    counter++;
    TCNT1 = 3035; // Tick 0.5 milisec | 2^16 - 62500
}

int main()
{
    DDRC = 1;
    DDRD = 0xFF;
    PORTC = 0x00;
    TCNT1 = 3035; // Tick 0.5 milisec | 2^16 - 62500
    TIMSK |= (1 << TOIE1); // Enable Timer1
    TCCR1B |= (1 << CS11); // Clk/8 Prescaler

    sei();

    while (true)
    {
    }

    return 0;
}
