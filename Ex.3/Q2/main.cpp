#include <avr/io.h>
#include <avr/interrupt.h>

int lcdNum = 0;
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

ISR(TIMER0_OVF_vect)
{
    switch (lcdNum)
    {
        case 0:
            PORTD = numTo7Seg(counter % 10);
            PORTC = 7;
            break;
        case 1:
            PORTD = numTo7Seg((counter / 10) % 10);
            PORTC = 11;
            break;
        case 2:
            PORTD = numTo7Seg((counter / 100) % 10);
            PORTC = 13;
            break;
        case 3:
            PORTD = numTo7Seg((counter / 1000) % 10);
            PORTC = 14;
            break;
    }
    lcdNum++;
    lcdNum %= 4;
}

ISR(TIMER1_OVF_vect)
{
    counter++;
    counter %= 10000;
    TCNT1 = 3035; // Tick 0.5 milisec | 2^16 - 62500
}

int main()
{
    DDRC = 0x0F;
    DDRD = 0x7F;
    PORTC = 0x00;
    TCNT0 = 254; // 2^8 - 1
    TCNT1 = 3035; // Tick 0.5 milisec | 2^16 - 62500
    TCCR1B |= (1 << CS11); // Clk/8 Prescaler
    TCCR0 |= (1 << CS01);  // Clk/8 Prescaler
    TIMSK |= (1 << TOIE1) | (1 << TOIE0); // Enable Timer1 And Timer0

    sei();

    while (true)
    {
    }

    return 0;
}
