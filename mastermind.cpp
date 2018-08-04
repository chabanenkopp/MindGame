// MasterMind.cpp : Defines the entry point for the console application.
//

#include <Arduino.h>

#include "mastermind.h"
#include "lcd_wrapper.h"

char generate_symbol()
{
    return '0' + rand() % 10;
}

char * generate_code(bool repeat, int length)
{
    if(!repeat && length > 10)
        return NULL;

    if(!length)
        return NULL;

    char * result = (char *)malloc(length + 1);

    result[0] = generate_symbol();

    for(int i = 1; i < length; i++) {
        char c = generate_symbol();

        if(repeat)
            result[i] = c;

        else {
            char * found;

            result[i] = 0;  // set zero at the end of current string
            found = strchr(result, c);

            while(found) {
                c = generate_symbol();
                found = strchr(result, c);
            }

            result[i] = c;
        }
    }

    result[length] = 0;
    return result;
}

void get_score(const char * secret, const char * guess, int * peg_a, int * peg_b)
{
    int a = 0;
    int b = 0;

    for(const char * s = secret, * g = guess; *s; s++, g++) {
        if(*s == *g)
            a++;
    }

    for(const char * g = guess; *g; g++) {
        const char * found = strchr(secret, *g);

        if(found)
            b++;
    }

    *peg_a = a;
    *peg_b = b - a;
}


void turn_off_leds()
{
    digitalWrite(LED_RED_1, LOW);
    digitalWrite(LED_RED_2, LOW);
    digitalWrite(LED_RED_3, LOW);
    digitalWrite(LED_RED_4, LOW);
    digitalWrite(LED_BLUE_1, LOW);
    digitalWrite(LED_BLUE_2, LOW);
    digitalWrite(LED_BLUE_3, LOW);
    digitalWrite(LED_BLUE_4, LOW);
}

void turn_on_leds()
{
    digitalWrite(LED_RED_1, HIGH);
    digitalWrite(LED_RED_2, HIGH);
    digitalWrite(LED_RED_3, HIGH);
    digitalWrite(LED_RED_4, HIGH);
    digitalWrite(LED_BLUE_1, HIGH);
    digitalWrite(LED_BLUE_2, HIGH);
    digitalWrite(LED_BLUE_3, HIGH);
    digitalWrite(LED_BLUE_4, HIGH);
}

void render_leds(const int peg_a, const int peg_b)
{
    turn_off_leds();

    switch(peg_a) {
    case 4:
        digitalWrite(LED_RED_4, HIGH);

    case 3:
        digitalWrite(LED_RED_3, HIGH);

    case 2:
        digitalWrite(LED_RED_2, HIGH);

    case 1:
        digitalWrite(LED_RED_1, HIGH);
    }

    switch(peg_b) {
    case 4:
        digitalWrite(LED_BLUE_4, HIGH);

    case 3:
        digitalWrite(LED_BLUE_3 + peg_a * 2, HIGH);

    case 2:
        digitalWrite(LED_BLUE_2 + peg_a * 2, HIGH);

    case 1:
        digitalWrite(LED_BLUE_1 + peg_a * 2, HIGH);
    }
}

void waitButtonForPress(uint8_t btn)
{
    while(digitalRead(btn) == LOW) {}
}

void waitButtonForRelease(uint8_t btn)
{
    while(digitalRead(btn) == HIGH) {}
}

void waitButton(uint8_t btn)
{
    waitButtonForPress(btn);
    waitButtonForRelease(btn);
}

char numberToDigit(int n)
{
    return (n % 10) + '0';
}

void print_score(int guessN, char * guess, int peg_a, int peg_b)
{
    lcd_set_cursor(0, 0);

    char guessNstr[] = "00: ";
    guessNstr[0] = numberToDigit(guessN / 10);
    guessNstr[1] = numberToDigit(guessN);

    lcd_print(guessNstr);
    lcd_print(guess);
    lcd_print(" ");

    char scoreStr[] = "0A0B";
    scoreStr[0] = numberToDigit(peg_a);
    scoreStr[2] = numberToDigit(peg_b);
    lcd_print(scoreStr);
}

void render_history(char * secret, char ** history, const int entry_nr)
{
    int peg_a, peg_b;
    get_score(secret, history[entry_nr], &peg_a, &peg_b);
    print_score(entry_nr + 1, history[entry_nr], peg_a, peg_b);
    render_leds(peg_a, peg_b);
}

void gameEnd(bool victory, char * secret, int historyLength, char ** history)
{
    int entryN = historyLength - 1;

    bool wasCombination;

    do {
        while(!digitalRead(BTN_ENTER_PIN)) {}

        wasCombination = false;

        while(digitalRead(BTN_ENTER_PIN)) {
            if(digitalRead(BTN_1_PIN)) {
                wasCombination = true;

                // wait BTN_1 for release
                while(digitalRead(BTN_1_PIN)) {}

                entryN--;

                if(entryN < 0)
                    entryN = historyLength - 1;

                render_history(secret, history, entryN);
            }

            if(digitalRead(BTN_2_PIN)) {
                wasCombination = true;

                // wait BTN_1 for release
                while(digitalRead(BTN_2_PIN)) {}

                entryN++;

                if(entryN > historyLength - 1)
                    entryN = 0;

                render_history(secret, history, entryN);
            }

        }
    }
    while(wasCombination);
}

void proccessButton(uint8_t btn, char * guess, int pos)
{
    if(digitalRead(btn) == HIGH) {
        waitButtonForRelease(btn);

        uint8_t digit = guess[pos] - '0';
        guess[pos] = ((digit + 1) % 10) + '0';
        lcd_print_at(1, 11, guess);
    }
}

void play_game(char * secret)
{
    lcd_clear();
    turn_off_leds();

    lcd_print("Welcome to");
    lcd_set_cursor(1, 0);
    lcd_print("MasterMind!");

    waitButton(BTN_ENTER_PIN);
    lcd_clear();
    lcd_print("I guess a number");

    typedef char Guess[5];
    Guess historyMem[10];
    char * history[10];

    for(int i = 0; i < 10; i++)
        history[i] = historyMem[i];

    int peg_a, peg_b;

    for(int guessN = 1; guessN <= 10; guessN++) {
        int entry_nr = guessN - 2;

        lcd_set_cursor(1, 0);
        lcd_print("Your guess: ");

        char * guess = history[guessN - 1];

        memset(guess, '0', 4);
        guess[4] = 0;

        lcd_print_at(1, 11, guess);

        bool wasCombination;

        do {
            while(!digitalRead(BTN_ENTER_PIN)) {
                proccessButton(BTN_1_PIN, guess, 0);
                proccessButton(BTN_2_PIN, guess, 1);
                proccessButton(BTN_3_PIN, guess, 2);
                proccessButton(BTN_4_PIN, guess, 3);
            }

            wasCombination = false;

            while(digitalRead(BTN_ENTER_PIN)) {
                if(digitalRead(BTN_1_PIN)) {
                    wasCombination = true;
                    waitButtonForRelease(BTN_1_PIN);

                    if(guessN > 2) {
                        entry_nr--;

                        if(entry_nr < 0)
                            entry_nr = guessN - 2;

                        render_history(secret, history, entry_nr);
                    }
                }
            }
        }
        while(wasCombination);

        get_score(secret, guess, &peg_a, &peg_b);

        lcd_clear();
        print_score(guessN, guess, peg_a, peg_b);

        if(peg_a == 4) {
            lcd_print_at(1, 0, "Well done!");
            render_leds(peg_a, peg_b);
            gameEnd(true, secret, guessN, history);
            return;
        }

        render_leds(peg_a, peg_b);
    }

    lcd_print_at(1, 0, "Fail,secret:");
    lcd_print(secret);

    gameEnd(false, secret, 10, history);
}


