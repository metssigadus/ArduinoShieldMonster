//Sample using LiquidCrystal library
#include <LiquidCrystal.h>
#include <SPI.h>
#include <Ethernet.h>

#include "/home/n0ne/sketchbook/EtherLCDclient/Glyph.h"
// https://github.com/arcao/Syslog/
#include "/home/n0ne/sketchbook/EtherLCDclient/Syslog.h"
// #include "/home/n0ne/sketchbook/EtherLCDclient/Syslog.cpp"

/*******************************************************

This program will test the buttons
available on the Arduino LCD shield

We use a *MODIFIED* 6-button shield with the reset button joined to the ladder, too.
Adapted from the 5-button code:  Mark Bramwell, July 2010
and from EtherCLient code by Tom Igoe and Arturo Guadalupi

An orgy of LCD tricks stolen here and there.
Unfinished syslog routines

Glyph programming 2017-03-07
Tested and not fully debugged out 2017-06-10

********************************************************/
 
// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 2, 5, 6, 7);
 
// need to set according your HW mods

int dimmerLine = 3; // e. g. A3
int buttonLine = 17; // e. g. pin 17

// button descriptions
#define btnSELECT 0
#define btnRIGHT  1
#define btnUP     2
#define btnDOWN   3
#define btnLEFT   4
#define btnMENU   5
#define btnNONE   6

int DelayTime=1200;
String versionNo = "2017-06-11T0110";

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xAF, 0x12, 0x34
};



// trivia
 int iteration = 1;
 int adc_key_in = 0;

// ------------------------------------------
// FUNCTIONS

void dimmerSoft ()
    // https://www.freetronics.com.au/pages/lcd-keypad-shield-backlight-control
{
  int i;
    for (i = 255; i > 0; i--)
  {
    analogWrite(dimmerLine, i);
    delay(15);
  }
  for (i = 0; i < 256; i++)
  {
    analogWrite(dimmerLine, i);
    delay(15);
  }
}
// ------------------------------------------

void dimmerHard ()
{
  analogWrite(dimmerLine,255);
  delay(DelayTime/4);
  analogWrite(dimmerLine,0);
  delay(DelayTime/4);
  analogWrite(dimmerLine,255);
  delay(DelayTime/4);
  analogWrite(dimmerLine,0);
  delay(DelayTime/4);
  analogWrite(dimmerLine,255);
  delay(DelayTime/4);
  analogWrite(dimmerLine,0);
  delay(DelayTime/4);
  analogWrite(dimmerLine,127);
  delay(DelayTime);
}
// ------------------------------------------
void showIPAddress()
/* Reproduce IP address on 16-char-per-line LCD
   We load an "IP" glyph into the LCD to spare chars) */
{
  lcd.write(3); // Print the IP glyph (1 char)
  lcd.print(Ethernet.localIP()); // (max 12+3 chars)
}


// ------------------------------------------

/*  read the ADC input and translate voltage intervals into button names */
int read_LCD_buttons()
{
 int analogLine = (buttonLine - 14); // only valid within pins 14-19
 adc_key_in = analogRead(buttonLine);      // read the value from ADC A0 line
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1500) return btnNONE; // 1-st (fastest) choice b/c most of time no button pushed
 if (adc_key_in < 50)   return btnSELECT; 
 if (adc_key_in < 200)  return btnRIGHT;  
 if (adc_key_in < 400)  return btnUP; 
 if (adc_key_in < 580)  return btnDOWN; 
 if (adc_key_in < 750)  return btnLEFT; 
 if (adc_key_in < 911)  return btnMENU;   
 return btnNONE;  // when all others fail, return this...
}

// -------------------------------
void loadGlyphs()
{
   // load our proprietary avatars into LCD char memory
 lcd.createChar(1, smiley); // a bitmap from the above byte array
 lcd.createChar(2, antismiley); // yet another bitmap
 lcd.createChar(3, ipglyph); // yet another bitmap
}

// -------------------------------

  // Last but not least - setting up syslog
    EthernetClient client; // Initialize the Ethernet client library
    EthernetUDP udpClient;
    Syslog syslog(udpClient, SYSLOG_PROTO_IETF); // https://github.com/arcao/Syslog/


void setup()
{

 loadGlyphs(); // into LCD character memory (max 8 glyphs)
 
 lcd.begin(16, 2);              // start the library only after avatars preloaded
 lcd.setCursor(0,1);
 lcd.print('v' + versionNo);
 lcd.setCursor(0,0);
 lcd.write(1); // Print the Smiley
  delay(DelayTime);

  analogWrite(dimmerLine,127);
  lcd.print(".Intro Time."); // show a little intro movie 
  delay(DelayTime);
  lcd.print(".");
    delay(DelayTime);
  lcd.print(".");
    delay(DelayTime);
  lcd.write(1);
  delay(DelayTime);
  delay(DelayTime);
 
  dimmerSoft(); // Playing with dimming
  dimmerHard(); // Playing with dimming
  

// ----------------------------------------
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    lcd.print("DHCP op failed!");
    // no point in carrying on, so do nothing forevermore:
    for (;;)
      ;
  }
  // print your local IP address:
  lcd.clear();
  lcd.setCursor(0,0);
  showIPAddress(); // we use a glyph to manage it on a 16 char line
  delay(DelayTime*4);
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Push the button!"); // Explain the task
  
  // https://github.com/arcao/Syslog
  syslog.server("192.168.2.52", 51400);
  syslog.deviceHostname("Arduino");
  syslog.appName("test");
  syslog.defaultPriority(LOG_KERN);
  
}


// -------------------------------

void loop()
{
 
  // DOES NOT WORK, WILL HANG THE HARDWARE - syslog.log(LOG_INFO, "Begin loop");
 
  lcd.setCursor(0,1);
 
 int lcd_key = 0;

// in theory it is not decent to push a 10 digit decimal number to 7 only digits, but ;)
 lcd.setCursor(9,1);            // move cursor to second line "1" and 9 spaces over
 lcd.print(millis()/1000);      // display the lifetime of the world, in seconds
 
 lcd.setCursor(0,1);            // move to the begining of the second line
 lcd_key = read_LCD_buttons();  // read the button voltage on ADC line A0
 
 switch (lcd_key)               // find out which button was pushed
 {
   case btnMENU:
     {
     lcd.print("MENU    ");
     break;
     }
   case btnLEFT:
     {
     lcd.print("LEFT    ");
     break;
     }
   case btnRIGHT:
     {
     lcd.print("RIGHT   ");
     break;
     }
   case btnUP:
     {
     lcd.print("UP      ");
     break;
     }
   case btnDOWN:
     {
     lcd.print("DOWN    ");
     break;
     }
   case btnSELECT:
     {
     lcd.print("SELECT  ");
     break;
     }
     case btnNONE:
     {
     lcd.write(2); // Preconcatenate unhappy face to the message
     lcd.print(" Nichtz ");
     break;
     }
 }
   iteration++;
 }
