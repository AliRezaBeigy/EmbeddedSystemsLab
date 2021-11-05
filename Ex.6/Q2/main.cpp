#include <stdio.h>
#include <avr/io.h>
#include "../LCD.h"
#include <avr/interrupt.h>

int max_temp = 20, h = 5, mode = 0;

ISR(INT2_vect)
{
    // Change Mode
    if ((PINB & (1 << PIN5)) > 0)
        mode ^= 1;
    // Up Min
    if (mode && (PINB & (1 << PIN4)) > 0)
        h++;
    // Down Min
    if (mode && (PINB & (1 << PIN6)) > 0)
        h--;
    // Up Max
    if (!mode && (PINB & (1 << PIN4)) > 0)
        max_temp++;
    // Down Max
    if (!mode && (PINB & (1 << PIN6)) > 0)
        max_temp--;
}

int main()
{
    LCD_Init();

    DDRB = 0x80;

    ADCSRA = 1 << ADEN | 1 << ADPS2 | 1 << ADPS1 | 1 << ADPS0; /* Enable ADC, with freq/128 */
    ADMUX = 1 << REFS0;                                        /* Vref: Avcc, ADC channel: 0 */

    GICR |= 1 << INT2;  /* Enable INT2 interupt */
    MCUCSR |= 1 << ISC2; /* Enable intrupt request on rising edge of INT2 */

    sei();

    while (1)
    {
        ADCSRA |= 1 << ADSC;

        while ((ADCSRA & (1 << ADIF)) == 0)
            ;

        int celsius = ADCW;

        LCD_Command(0x01);
        LCD_Command(0x80);
        char buffer[20];
        sprintf(buffer, "%d C", celsius);
        LCD_String(buffer);

        LCD_Command(0xC0);
        sprintf(buffer, "Max:%d, H:%d", max_temp, h);
        LCD_String(buffer);

        if (celsius >= max_temp)
            PORTB = 1 << PORTB7;
        if (celsius <= max_temp - h)
            PORTB = 0;

        _delay_ms(250);
    }

    return 0;
}