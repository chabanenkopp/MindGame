//#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>

#define I2C_ADDR    0x3F // <<----- Add your address here.  Find it from I2C Scanner
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7

//int n = 1;

LiquidCrystal_I2C  lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);

void lcd_init()
{
  lcd.begin (16, 2); //  <<----- My LCD is 16x2

  // Switch on the backlight
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.home (); // go home

  //lcd.print("Welcome!");
}

void lcd_clear() {
  lcd.clear();
}

void lcd_set_cursor(int y, int x) {
  lcd.setCursor(x, y);
}

void lcd_print(char* text) {
  lcd.print(text);
  //Serial.println(text);
}

void lcd_print_at(int y, int x, char* text) {
  lcd_set_cursor(y, x);
  lcd_print(text);
}


