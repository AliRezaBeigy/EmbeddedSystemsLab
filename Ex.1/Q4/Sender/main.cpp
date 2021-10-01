#define F_CPU 8000000UL /* Define CPU Frequency e.g. here 8MHz */

#include <string.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/eeprom.h>

int main()
{
    char text[200] = "12345 long established fact that a reader will be distracted by the readable content of a page when looking at its layout. The point of using Lorem Ipsum is that it has a more-or-less normal distri";

    DDRB = 0xFF;
    PORTB = 0;

    eeprom_busy_wait();
    eeprom_write_block(text, 0, strlen(text));
    
    _delay_ms(1);

    for (int i = 0; text[i] != 0; i++)
    {
        PORTB = text[i];
        _delay_ms(1);
    }

    PORTB = 0;

    return 0;
}
