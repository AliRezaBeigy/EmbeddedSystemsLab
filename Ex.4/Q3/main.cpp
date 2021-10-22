#include <stdio.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <util/atomic.h>

#define LCD_Data_Dir DDRB      /* Define LCD data port direction */
#define LCD_Command_Dir DDRA   /* Define LCD command port direction register */
#define LCD_Data_Port PORTB    /* Define LCD data port */
#define LCD_Command_Port PORTA /* Define LCD data port */
#define RS PC0                 /* Define Register Select (data/command reg.)pin */
#define RW PC1                 /* Define Read/Write signal pin */
#define EN PC2                 /* Define Enable signal pin */

void LCD_Command(unsigned char cmnd)
{
    LCD_Data_Port = cmnd;
    LCD_Command_Port &= ~(1 << RS); /* RS=0 command reg. */
    LCD_Command_Port &= ~(1 << RW); /* RW=0 Write operation */
    LCD_Command_Port |= (1 << EN);  /* Enable pulse */
    _delay_us(1);
    LCD_Command_Port &= ~(1 << EN);
    _delay_ms(3);
}

void LCD_Init(void) /* LCD Initialize function */
{
    LCD_Command_Dir = 0xFF; /* Make LCD command port direction as o/p */
    LCD_Data_Dir = 0xFF;    /* Make LCD data port direction as o/p */

    _delay_ms(20);     /* LCD Power ON delay always >15ms */
    LCD_Command(0x38); /* Initialization of 16X2 LCD in 8bit mode */
    LCD_Command(0x0C); /* Display ON Cursor OFF */
    LCD_Command(0x06); /* Auto Increment cursor */
    LCD_Command(0x01); /* clear display */
    LCD_Command(0x80); /* cursor at home position */
}

void LCD_Char(unsigned char char_data)
{
    LCD_Data_Port = char_data;
    LCD_Command_Port |= (1 << RS);  /* RS=1 Data reg. */
    LCD_Command_Port &= ~(1 << RW); /* RW=0 write operation */
    LCD_Command_Port |= (1 << EN);  /* Enable Pulse */
    _delay_us(1);
    LCD_Command_Port &= ~(1 << EN);
    _delay_ms(1);
}

void LCD_String(char *str)
{
    for (int i = 0; str[i] != 0; i++)
        LCD_Char(str[i]); /* call LCD data write */
}

int time = 0;
bool alarmStart = false;
unsigned int counter = 0;
int mode = 0; // HOUR(0), MINUTE(1), SECOUND(2)

ISR(TIMER2_OVF_vect)
{
    counter++;

    if (counter == 61)
    {
        counter = 0;
        return;
    }

    if (alarmStart && time == 0)
    {
        return;
    }

    if (counter == 9)
    {
        LCD_Command(0x01); /* clear display */
        if (alarmStart)
        {
            time--;

            short hour = time / 3600;
            short minute = (time % 3600) / 60;
            short second = (time % 3600) % 60;

            if (time == 0)
            {
                PORTC = 1 << PIN0;
                LCD_String("! Alarm Raised !");
            }
            else
            {
                char buffer[20];
                sprintf(buffer, "%02d:%02d:%02d", hour, minute, second);
                LCD_String(buffer);
            }
        }
        else
        {
            short hour = time / 3600;
            short minute = (time % 3600) / 60;
            short second = (time % 3600) % 60;

            char buffer[20];
            sprintf(buffer, "%02d:%02d:%02d", hour, minute, second);
            LCD_String(buffer);
        }
    }
}

ISR(INT0_vect)
{
    // Up Time
    if ((PINC & (1 << PIN1)) > 0)
    {
        if (mode == 0)
        {
            time += 3600;
        }
        if (mode == 1)
        {
            time += 60;
        }
        if (mode == 2)
        {
            time += 1;
        }
    }
    // Mode Setup
    if ((PINC & (1 << PIN2)) > 0)
    {
        mode = (mode + 1) % 3;
    }
    // Down Time
    if ((PINC & (1 << PIN3)) > 0)
    {
        if (mode == 0)
        {
            time -= 3600;
        }
        if (mode == 1)
        {
            time -= 60;
        }
        if (mode == 2)
        {
            time -= 1;
        }
    }
    // Start/Restart Alarm
    if ((PINC & (1 << PIN4)) > 0)
    {
        if (alarmStart)
        {
            time = 0;
            PORTC = 0;
            alarmStart = false;
        }
        else
        {
            alarmStart = true;
        }
    }
}

int main()
{
    LCD_Init(); /* Initialize LCD */

    DDRC = 1 << PIN0;

    // TIMSK |= (1 << TOIE1); /* Enable Timer1 */
    // TCCR1B |= (1 << CS11); /* Clk/8 Prescaler */
    TIMSK = 1 << TOIE2; /* Enable Timer 2 */
    TCCR2 = 1 << CS22;  /* Set Timer 2 Prescaller */

    GICR = 1 << INT0;                /* Enable INT0 */
    MCUCR = 1 << ISC01 | 1 << ISC00; /* Trigger INT0 on raising edge */

    sei();

    while (1)
    {
    }

    return 0;
}