#define F_CPU 8000000UL /* Define CPU Frequency e.g. here 8MHz */

#include <string.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/eeprom.h>

int main()
{
    char text[200] = "12345 long established fact that a reader will be distracted by the readable content of a page when looking at its layout. The point of using Lorem Ipsum is that it has a more-or-less normal distri";

    DDRB = 1;
    PORTB = 0;

    eeprom_busy_wait();
    eeprom_write_block(text, 0, strlen(text));

    PORTB = 1;
    _delay_us(100);

    for (int i = 0; text[i] != 0; i++)
    {
        for (int j = 0; j != 8; j++)
        {
            PORTB = (text[i] >> j) & 1;
            _delay_us(100);
        }
    }

    PORTB = 0;

    return 0;
}
