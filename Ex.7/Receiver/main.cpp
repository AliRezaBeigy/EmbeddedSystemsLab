#include <stdio.h>
#include <avr/io.h>
#include <string.h>
#include "../LCD.h"
#include <util/delay.h>
#include <avr/interrupt.h>

int mode = 0; // LOGIN(0), INFO(1), PASSWORD(2), WELCOME(3), HOME_1(4), ADD_USER_ID(5), ADD_USER_PASSWORD(6), HOME_2(7), EDIT_DATE(8)

int last_button = 0b00001111;

char input[6] = "";

char login_user_id[6] = "";
char user_count = 1;
char user_ids[3][6] = {"10"};
char user_names[3][6] = {"Admin"};
char user_passwords[3][6] = {"123"};

unsigned long long time = 0;
char time_mode = 0; // YEAR(0), MONTH(1), DAY(2), HOUR(3), MINUTE(4), SECOND(5)

void handleKeyboard(char *key)
{
    // Left Change Time Cursor
    if (key == "+" && mode == 1)
    {
        mode = 8;
        LCD_Command(0x01);
        return;
    }
    // Left Change Time Cursor
    if (key == "4" && mode == 8)
    {
        time_mode = time_mode - 1;
        if (time_mode < 0)
        {
            time_mode = 0;
        }
        return;
    }
    // Right Change Time Cursor
    if (key == "6" && mode == 8)
    {
        time_mode = (time_mode + 1) % 6;
        return;
    }
    // Up Down Edit Date
    if ((key == "+" || key == "-") && mode == 8)
    {
        unsigned long long diff = 0;
        if (time_mode == 0)
        {
            diff = 3600 * 24 * 30 * 12;
        }
        if (time_mode == 1)
        {
            diff = 3600 * 24 * 30;
        }
        if (time_mode == 2)
        {
            diff = 3600 * 24;
        }
        if (time_mode == 3)
        {
            diff = 3600;
        }
        if (time_mode == 4)
        {
            diff = 60;
        }
        if (time_mode == 5)
        {
            diff = 1;
        }

        if (key == "+")
        {
            time += diff;
        }
        if (key == "-")
        {
            time += diff;
        }
        return;
    }
    if (key == "*" && mode == 8)
    {
        LCD_Command(0x01);
        LCD_Command(0x80);
        LCD_String("1. Add User");
        LCD_Command(0xC0);
        LCD_String("2. Delete User");
        LCD_Command(0x94);
        LCD_String("3. Show Info");
        LCD_Command(0xD4);
        LCD_String("+ Next");
        sprintf(input, "%s", "\0");
        mode = 4;
        return;
    }

    // Navigate to Add User
    if (key == "1" && mode == 4)
    {
        LCD_Command(0x01);
        LCD_Command(0x80);
        if (user_count == 2)
        {
            LCD_String("Memory Limit.");
            LCD_Command(0xD4);
            LCD_String("Press =");
            mode = 3;
        }
        else
        {
            LCD_String("Enter ID:");
            mode = 5;
        }
        return;
    }

    // Navigate to Info
    if (key == "3" && mode == 4)
    {
        LCD_Command(0x01);
        mode = 1;
        return;
    }

    // Logout User
    if (key == "6" && mode == 7)
    {
        LCD_Command(0x01);
        LCD_Command(0x80);
        LCD_String("Enter User Id: ");
        mode = 0;
        return;
    }

    // Navigate secound home page
    if (key == "+" && mode == 4)
    {
        LCD_Command(0x01);
        LCD_Command(0x80);
        LCD_String("4. Start Motor");
        LCD_Command(0xC0);
        LCD_String("5. Edit Date");
        LCD_Command(0x94);
        LCD_String("6. Logout");
        LCD_Command(0xD4);
        LCD_String("- Back");
        sprintf(input, "%s", "\0");
        mode = 7;
        return;
    }

    // Add User ID
    if (key == "=" && mode == 5)
    {
        strcpy(user_ids[user_count], input);
        LCD_Command(0x01);
        LCD_Command(0x80);
        LCD_String("Enter Password:");
        mode = 6;
        sprintf(input, "%s", "\0");
        return;
    }

    // Add User Password
    if (key == "=" && mode == 6)
    {
        strcpy(user_passwords[user_count], input);
        strcpy(user_names[user_count], "User");
        user_count += 1;

        LCD_Command(0xC0);
        LCD_String("User Added.");
        LCD_Command(0x94);
        LCD_String("Press =");
        PORTD = 1 << PIN6;

        mode = 3;
        sprintf(input, "%s", "\0");
        return;
    }

    // GOTO HOME
    if ((key == "=" && mode == 3) || (key == "-" && mode == 7))
    {
        PORTD = 0;
        LCD_Command(0x01);
        LCD_Command(0x80);
        LCD_String("1. Add User");
        LCD_Command(0xC0);
        LCD_String("2. Delete User");
        LCD_Command(0x94);
        LCD_String("3. Show Info");
        LCD_Command(0xD4);
        LCD_String("+ Next");
        sprintf(input, "%s", "\0");
        mode = 4;
        return;
    }

    // Enter Username
    if (key == "=" && mode == 0)
    {
        for (char i = 0; i < user_count; i++)
        {
            if (strcmp(user_ids[i], input) == 0)
            {
                LCD_Command(0x01);
                LCD_Command(0x80);
                LCD_String("Enter Password:");
                sprintf(input, "%s", "\0");
                sprintf(login_user_id, "%s", user_ids[i]);
                mode = 2;
                PORTD = 1 << PIN6;
                _delay_ms(100);
                PORTD = 0;
            }
        }
        if (mode == 0)
        {
            PORTD = 1 << PIN7;
            _delay_ms(1000);
            PORTD = 0;
        }
        return;
    }

    // Enter Password
    if (key == "=" && mode == 2)
    {
        for (char i = 0; i < user_count; i++)
        {
            if (strcmp(user_ids[i], login_user_id) == 0 && strcmp(user_passwords[i], input) == 0)
            {
                LCD_Command(0xC0);
                LCD_String("Welcome ");
                LCD_String(user_names[i]);

                LCD_Command(0x94);
                LCD_String("Press =");
                PORTD = 1 << PIN6;

                mode = 3;
            }
        }
        if (mode == 2)
        {
            PORTD = 1 << PIN7;
            _delay_ms(1000);
            PORTD = 0;
        }
        return;
    }

    // Clear Button
    if (key == "ON" && (mode == 0 || mode == 2 || mode == 3 || mode == 4 || mode == 5 || mode == 6))
    {
        char inputSize = strlen(input);

        for (char i = 0; i < inputSize; i++)
            LCD_Command(0x10); /* Go Cursor Left */
        for (char i = 0; i < inputSize; i++)
        {
            char space[] = {" "[0], " "[1]};
            LCD_String(space);
        }
        for (char i = 0; i < inputSize; i++)
            LCD_Command(0x10); /* Go Cursor Left */

        sprintf(input, "%s", "\0");
        return;
    }

    // Typing
    if (mode == 0 || mode == 2 || mode == 4 || mode == 5 || mode == 6)
    {
        sprintf(input, "%s%s", input, key);
        LCD_String(key);
    }
}

char *table[16] = {
    "7", "8", "9", "/",
    "4", "5", "6", "*",
    "1", "2", "3", "-",
    "ON", "0", "=", "+"};

void keypad_init()
{
    DDRC = 0x0f;
    PORTC = 0xff;
}

char key_released()
{
    PORTC = 0x0f;
    _delay_ms(3);
    if ((PINC & 0xf0) == 0x00)
        return 1;
    else
        return 0;
}

char key_pressed()
{
    PORTC = 0x0f;
    _delay_ms(3);
    if ((PINC & 0xf0) != 0x00)
    {
        _delay_ms(20);
        if ((PINC & 0xf0) != 0x00)
            return 1;
    }
    return 0;
}

char *key_scan()
{
    if (key_pressed())
    {
        char *key = "";
        for (char i = 0; i < 4; i++)
        {
            PORTC = 1 << i;
            _delay_ms(3);
            if ((PINC & (1 << PIN4)) > 0)
                key = table[i * 4];
            if ((PINC & (1 << PIN5)) > 0)
                key = table[i * 4 + 1];
            if ((PINC & (1 << PIN6)) > 0)
                key = table[i * 4 + 2];
            if ((PINC & (1 << PIN7)) > 0)
                key = table[i * 4 + 3];
        }
        while (!key_released())
            ;
        return key;
    }
    else
        return "";
}

unsigned int counter = 0;

ISR(TIMER2_OVF_vect)
{
    counter++;

    if (counter == 61)
    {
        counter = 0;
        return;
    }

    if (counter == 9)
    {
        time++;
        if (mode == 1)
        {
            short year = time / (3600 * 24 * 30 * 12);
            short month = (time % (3600 * 24 * 30 * 12)) / (3600 * 24 * 30);
            short day = (time % (3600 * 24 * 30)) / (3600 * 24);
            short hour = (time % (3600 * 24)) / 3600;
            short minute = (time % 3600) / 60;
            short second = (time % 3600) % 60;
            char buffer[20];
            sprintf(buffer, "%04d/%02d/%02d %02d:%02d:%02d", year, month, day, hour, minute, second);
            LCD_Command(0x01);
            LCD_Command(0x80);
            LCD_String("Date Time:");

            LCD_Command(0xC0);
            LCD_String(buffer);
            LCD_Command(0xD4);
            LCD_String("X Home | + Edit Date");
        }

        if (mode == 8)
        {
            time--;
            short year = time / (3600 * 24 * 30 * 12);
            short month = (time % (3600 * 24 * 30 * 12)) / (3600 * 24 * 30);
            short day = (time % (3600 * 24 * 30)) / (3600 * 24);
            short hour = (time % (3600 * 24)) / 3600;
            short minute = (time % 3600) / 60;
            short second = (time % 3600) % 60;
            char buffer[20];

            if (time_mode == 0)
            {
                sprintf(buffer, "%04d/XX/XX XX:XX:XX", year);
            }
            if (time_mode == 1)
            {
                sprintf(buffer, "XXXX/%02d/XX XX:XX:XX", month);
            }
            if (time_mode == 2)
            {
                sprintf(buffer, "XXXX/XX/%02d XX:XX:XX", day);
            }
            if (time_mode == 3)
            {
                sprintf(buffer, "XXXX/XX/XX %02d:XX:XX", hour);
            }
            if (time_mode == 4)
            {
                sprintf(buffer, "XXXX/XX/XX XX:%02d:XX", minute);
            }
            if (time_mode == 5)
            {
                sprintf(buffer, "XXXX/XX/XX XX:XX:%02d", second);
            }

            LCD_Command(0x80);
            LCD_String("   4 < Cursor > 6");
            LCD_Command(0xC0);
            LCD_String(buffer);
            LCD_Command(0x94);
            LCD_String("+ Up     |    Down -");
            LCD_Command(0xD4);
            LCD_String("X Home");
        }
    }
}
int main()
{
    LCD_Init();
    keypad_init();

    TIMSK = 1 << TOIE2; /* Enable Timer 2 */
    TCCR2 = 1 << CS22;  /* Set Timer 2 Prescaller */

    DDRD = 0b11000000;

    sei();

    LCD_String("Enter User Id: ");

    while (true)
    {

        char *str = key_scan();
        if (str != "")
        {
            handleKeyboard(str);
        }
    }
}
