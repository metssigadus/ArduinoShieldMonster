//Sample using LiquidCrystal library
#include <LiquidCrystal.h>
#include <SPI.h>
#include <Ethernet.h>
/*******************************************************

This program will test the buttons
available on the Arduino LCD shield

We use a *MODIFIED* 6-button shield with the reset button joined to the ladder, too.
Adapted from the 5-button code:  Mark Bramwell, July 2010
Char programming 2017-03-07
Tested 2017-06-10
The sceleton needs yet to be debugged and polished
********************************************************/


/* Web Server by Mellis Igoe Guadalupi

 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield. 

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional) */


/* Ether */

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0x12, 0x23
};
IPAddress ip(192, 168, 2, 231);

EthernetServer server(80); // Initialize the Ethernet server library

/* LCD */
// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 2, 5, 6, 7);
 
// define some values used by the panel and buttons
int adc_key_in = 0;
int dimmerLine = 3;

#define btnSELECT 0
#define btnRIGHT  1
#define btnUP     2
#define btnDOWN   3
#define btnLEFT   4
#define btnMENU   5
#define btnNONE   6

int DelayTime=1200;

// read the buttons
int read_LCD_buttons(int num)
{
 adc_key_in = num;      // read the value from ADC A0 line
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1500) return btnNONE; // 1-st (fastest) choice b/c most of time no button pushed
 if (adc_key_in < 30)   return btnSELECT; 
 if (adc_key_in < 210)  return btnRIGHT;  
 if (adc_key_in < 430)  return btnUP; 
 if (adc_key_in < 620)  return btnDOWN; 
 if (adc_key_in < 780)  return btnLEFT; 
 if (adc_key_in < 950)  return btnMENU;   
 return btnNONE;  // when all others fail, return this...
}


void dimmer ()
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



void setup()
{

  

  // Some inverse bitmaps to be loaded into char memory of the LCD
  byte smiley[8] = {
  0b11111,
  0b10101,
  0b11111,
  0b11011,
  0b11011,
  0b01110,
  0b10001,
  0b11111
};

  byte antismiley[8] = {
  0b11111,
  0b10101,
  0b11111,
  0b11011,
  0b11111,
  0b10001,
  0b01110,
  0b11111
};


    // load our proprietary avatars into LCD char memory
 lcd.createChar(6, smiley); // a bitmap from the above byte array
 lcd.createChar(7, antismiley); // yet another bitmap
 lcd.begin(16, 2);              // start the library only after avatars preloaded
 lcd.setCursor(0,0);
 lcd.write(6); // Print the Smiley
  delay(DelayTime);
 analogWrite(dimmerLine,127);

 
  lcd.print(".Intro Time."); // show a little intro movie 
  delay(DelayTime);
  lcd.print(".");
    delay(DelayTime);
  lcd.print(".");
  delay(DelayTime);

  lcd.write(6);
  delay(DelayTime);
  delay(DelayTime);
 
  dimmer(); // Playing with dimming
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

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Go Ether!");
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  lcd.setCursor(0,0);
  lcd.print("Go to Web Srv:");
  lcd.setCursor(0,1);
  lcd.print(Ethernet.localIP()); // (max 12+3 chars)
  delay(DelayTime *4);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Push the Buttons"); // Explain the task
  lcd.setCursor(0,1);
  lcd.write(6); // Print the Smiley
  lcd.print("Results on Web!"); // Explain the task
}
  
void loop()
{
 int lcd_key     = 0;



  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 1");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          
          for (int analogChannel = 3; analogChannel < 4; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            int decision = read_LCD_buttons (sensorReading);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is at value ");
            client.print(sensorReading);
            client.println("<br />");
            client.print("Decision is: ");
            client.print(decision);
            client.print(", Button ");
            
 switch (decision)               // find out which button was pushed
 {
   case btnMENU:
     {
     client.print("MENU    ");
     break;
     }
   case btnLEFT:
     {
     client.print("LEFT    ");
     break;
     }
   case btnRIGHT:
     {
     client.print("RIGHT   ");
     break;
     }
   case btnUP:
     {
     client.print("UP      ");
     break;
     }
   case btnDOWN:
     {
     client.print("DOWN    ");
     break;
     }
   case btnSELECT:
     {
     client.print("SELECT  ");
     break;
     }
     case btnNONE:
     {
     client.print(" NONE ");
     break;
     }
 }
            client.print(" has been pushed");
            client.println("<br />");
          }
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    
  }






 }

