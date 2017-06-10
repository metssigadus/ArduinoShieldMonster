// Tuning LCD Keypad Shield after the modification

#include <LiquidCrystal.h> // mandatory requirement
 
/*******************************************************
 
This program will test the buttons
available on the Arduino LCD shield
and measure the resulting ADC output

We use a *MODIFIED* 6-button shield with the reset button added to ladder, too.
Adapted from the 5-button code:  Mark Bramwell, July 2010 by unknown author ;)
Tested and debugged  2017-06-10.
Useful links:
https://meettechniek.info/embedded/arduino-analog.html

https://biskandar.wordpress.com/2012/10/17/request-counter-with-arduino-uno-ethernet-shi/

https://forum.arduino.cc/index.php?topic=91256.0

How to Debouce Six Buttons on One Analog Pin With Arduino - Electrical Engineering Stack Exchange -
https://electronics.stackexchange.com/questions/101409/how-to-debouce-six-buttons-on-one-analog-pin-with-arduino

Multiple Buttons on 1 analog pin -
http://forum.arduino.cc/index.php?topic=8558.0
********************************************************/
int DelayTime=1200;

// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 2, 5, 6, 7);

#define btnSELECT 0
#define btnRIGHT  1
#define btnUP     2
#define btnDOWN   3
#define btnLEFT   4
#define btnMENU   5
#define btnNONE   6

// The resistors after my mod :  0, 330, 620, 1K2, 2k2, 4k7 + VCC via 2k2
// A0 values via 2k2 to VCC   :  0, 130, 305, 503, 678, 824
// Tresholds                  : 50, 200, 400, 580, 750, 911 and max=1023

void setup()
{
 
 lcd.begin(16, 2);   // start the library with 2 lines 16 chars LCD
 lcd.setCursor(0,0); // Using FIRST line on LCD
 lcd.print("I was restarted ..."); // print a debug message
 delay(2*DelayTime);
 lcd.clear();

}


void loop()
{
 float vcc = 5.0;
 float voltage = 0.0;
 int snapshot = 0;
 
 lcd.setCursor(0,0);
 lcd.print("SLOW push needed");
 lcd.setCursor(0,1);            // Using second line of LCD
 lcd.print("ADC=");
 lcd.setCursor(4,1);
 
 snapshot = analogRead(3); // read the button value from A0 
 lcd.print(snapshot);
 voltage = ((vcc * snapshot) / 1023); //firuge out how much it is in Volts
 
 lcd.setCursor(9,1); // print data out
 lcd.print("U=");  
 lcd.print(voltage);
 lcd.print("V");
 
 delay(DelayTime);
 lcd.clear();
 
 }

