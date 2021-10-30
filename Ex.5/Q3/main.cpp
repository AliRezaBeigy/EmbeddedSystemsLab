#include <avr/io.h>
#include <avr/interrupt.h>

int t = 0;
unsigned int counter = 0;
unsigned int counter_2 = 0;
char T[] = {0b0101, 0b0110, 0b1010, 0b1001};

int mode = 0;  // Idle(0), Start(1)
int state = 0; // ClockwiseSpeedUp(0), ClockwiseSpeedDown(1), AntiClockwiseSpeedUp(2), AntiClockwiseSpeedDown(3)

const int MAX_SPEED = 5;
const int MIN_SPEED = -5;
int speed = 0;

ISR(TIMER2_OVF_vect)
{
    counter_2++;
    TCNT2 = 130;

    if (counter_2 == 350)
    {
        counter_2 = 0;

        if (mode == 0)
        {
            return;
        }

        if (state == 0 || state == 2)
        {
            if (speed < MAX_SPEED)
            {
                speed += 1;
            }
            else
            {
                state++;
                speed--;
            }
        }
        if (state == 1 || state == 3)
        {
            if (speed < MIN_SPEED)
            {
                if (state == 3)
                    state = 0;
                else
                    state++;
                speed++;
            }
            else
            {
                speed -= 1;
            }
        }
    }
}

ISR(TIMER0_OVF_vect)
{
    counter++;
    TCNT0 = 130;

    if (counter >= 5 - speed)
    {
        counter = 0;

        if (mode == 0)
        {
            return;
        }

        PORTA = T[t];
        if (state == 0 || state == 1)
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
    if (mode == 0)
    {
        speed = 0;
    }
}

int main()
{
    DDRA = 0xFF;

    PORTA = 0x00;

    TIMSK = 1 << TOIE0 | 1 << TOIE2; // Enable Timer0 And Timer2
    TCCR2 |= 1 << CS21;               // Clk/8 Prescaler
    TCCR0 |= 1 << CS01;              // Clk/8 Prescaler
    GICR = 1 << INT0;                // Enable INT0
    MCUCR = 1 << ISC01 | 1 << ISC00; // Trigger INT0 on raising edge

    sei();

    while (true)
    {
    }
}
