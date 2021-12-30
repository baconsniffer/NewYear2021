/*
LCD_I2C - Arduino library to control a 16x2 LCD via an I2C adapter based on PCF8574

Copyright(C) 2020 Blackhack <davidaristi.0504@gmail.com>

This program is free software : you can redistribute it and /or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see < https://www.gnu.org/licenses/>.
*/

#include <LCD_I2C.h>

// Define the area to exclude from snowflake placement
#define  XMIN1    7
#define  XMAX1   11
#define  XMIN2    5
#define  XMAX2   13
#define  BADY1    1
#define  BADY2    2

// define the pin numbers used for PWM output
#define PWM1      3
#define PWM2      5
#define PWM3      6
#define PWM4      9
#define PWM5     10
#define PWM6     11

// the delta to apply to PWM signal
#define PWMDELTA1  -4
#define PWMDELTA2  10
#define PWMDELTA3  18
#define PWMDELTA4  -8
#define PWMDELTA5  -3
#define PWMDELTA6   7

#define BASEDELAY  50
#define FLAKEDELAY 20

LCD_I2C lcd(0x27, 20, 4); // Default address of most PCF8574 modules, change according

uint8_t snow[8] =
{
    0b01000,
    0b11101,
    0b01011,
    0b00001,
    0b00100,
    0b01110,
    0b00100,
    0b10000
};

uint8_t snow2[8] =
{
    0b00100,
    0b01110,
    0b10100,
    0b00000,
    0b01001,
    0b11100,
    0b01000,
    0b00010
};

uint8_t snow3[8] =
{
    0b00010,
    0b00111,
    0b00010,
    0b10000,
    0b11000,
    0b10000,
    0b00010,
    0b00111,
};

uint8_t snow4[8] =
{
    0b00010,
    0b10001,
    0b00100,
    0b10000,
    0b00001,
    0b01000,
    0b00100,
    0b10000,
};

struct flake {
  int x, y;
  int oldx, oldy;
} _flake;

flake  flake1, flake2, flake3, flake4;

// A listing of the pins used
uint8_t pwmPin[] = {PWM1, PWM2, PWM3, PWM4, PWM5, PWM6 };

// A table to hold the PWM value of each pin
uint8_t pwmVal[6];

// A table of the delta to appy to each PWM value each iteration of the loop
int8_t pwmDelta[6] = {PWMDELTA1, PWMDELTA2, PWMDELTA3, PWMDELTA4, PWMDELTA5, PWMDELTA6 };

// Function prototypes
//void drawflake(flake* fk, int fl);
//void hideflake(flake* fk);
//void computeFlake(flake*);

int newx;
int newy;
int doFlake = 0;

void drawFlake(flake *fk, int fl) {
   lcd.setCursor(fk->x, fk->y);
   lcd.write(fl);
}

void hideFlake(flake *fk) {
   lcd.setCursor(fk->oldx, fk->oldy);
   lcd.print(" ");
}

void computeFlake(flake *fk) {
   // Save previous positions
   fk->oldx = fk->x;
   fk->oldy = fk->y;

   while(1) {
      newx = random(0, 20);
      newy = random(0,4);
      if ((newy ==0) || (newy == 3))
         break;
      if (((newx < XMIN1) || (newx > XMAX1)) && (newy == BADY1)) 
         break; 
      if (((newx < XMIN2) || (newx > XMAX2)) && (newy == BADY2)) 
         break; 
      }
   fk->x = newx;
   fk->y = newy;
}

void twinkle() {
   int i;

   for (i = 0; i < 6; i++) {
     pwmVal[i] += pwmDelta[i];
     if (pwmVal[i] <   0 ) { 
        pwmVal[i] = -pwmVal[i];
        pwmDelta[i] = -pwmDelta[i];
     }
     if (pwmVal[i] > 255 ) {
        pwmVal[i] = 255;
        pwmDelta[i] = -pwmDelta[i];
     }
     analogWrite(pwmPin[i], pwmVal[i]);
   }
}

void setup()
{
    int i;
    
    lcd.begin();
    lcd.backlight();

    lcd.createChar(0, snow);
    lcd.createChar(1, snow2);
    lcd.createChar(2, snow3);
    lcd.createChar(3, snow4);

    lcd.setCursor(7, 1);
    lcd.print("HAPPY");
    lcd.setCursor(5, 2);
    lcd.print("NEW YEAR!");

    // Establish initial values for the PWM outputs, setup the pins as PWM outputs
    for (i = 0; i < 6; i++) {
       pwmVal[i] = random(0, 128);
       pinMode(pwmPin[i], OUTPUT);
       analogWrite(pwmPin[i], pwmVal[i]);
    }

}

void loop()
{
   flake1.x = flake1.oldx = 0; flake1.y =flake1.oldy = 0;
   drawFlake(&flake1, 0);

   flake2.x = flake2.oldx = 19; flake2.y = flake2.oldy = 0;
   drawFlake(&flake2, 1);

   flake3.x = flake3.oldx = 0; flake3.y = flake3.oldy = 3;
   drawFlake(&flake3, 2);

   flake4.x = flake4.oldx = 19, flake4.y = flake4.oldy = 3;
   drawFlake(&flake4, 3);

   while(1) {
      // Wait a moment
      delay(BASEDELAY);
      
      // Update the twinkling lights
      twinkle();

      if (doFlake++ > FLAKEDELAY) {
         doFlake = 0;     
         
         // Erase old flakes
         hideFlake(&flake1);
         hideFlake(&flake2);
         hideFlake(&flake3);
         hideFlake(&flake4);

         // Compute new flake positions
         computeFlake(&flake1);
         computeFlake(&flake2);
         computeFlake(&flake3);
         computeFlake(&flake4);

         // Draw flakes inn new positions
         drawFlake(&flake1, 0);
         drawFlake(&flake2, 1);
         drawFlake(&flake3, 2);
         drawFlake(&flake4, 3);
      }
   }
}
