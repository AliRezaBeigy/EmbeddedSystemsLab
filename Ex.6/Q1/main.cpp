#include <stdio.h>
#include "../LCD.h"
#include <avr/io.h>

int main()
{
    LCD_Init();

    ADCSRA = 1 << ADEN | 1 << ADPS2 | 1 << ADPS1 | 1 << ADPS0; /* Enable ADC, with freq/128 */
    ADMUX = 1 << REFS0;                                        /* Vref: Avcc, ADC channel: 0 */


    while (true)
    {
        ADCSRA |= 1 << ADSC;

        while ((ADCSRA & (1 << ADIF)) == 0)
            ;

        int celsius = ADCW;
        int fahrenheit = (int)((celsius * 9 / 5) + 32);

        LCD_Command(0x01);
        LCD_Command(0x80);
        char buffer[20];
        sprintf(buffer, "%d C   |   %d F", celsius, fahrenheit);
        LCD_String(buffer);
        _delay_ms(250);
    }
}
