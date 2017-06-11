/*
  Src: https://arduino-info.wikispaces.com/MultipleTemperatureSensorsToLCD
  (c) terry@yourduino.com 
  Modified by me for my own monster HW.
  
  Multiple DS18B20 Temperature Sensors
  Displayed on 2x16 character LCD display
  */

/*-----( Import libraries )-----*/
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h> 

// When
String versionNo = "2017-06-12T0119";

/*-----( Declare Constants and Pin Numbers )-----
  Seems like #define's take less space than const int's ?
  */

// Data wire is plugged into port A2 on the Arduino (can be changed)
#define ONE_WIRE_BUS A2

//--- LCD pin assignments
#define pin_RS 8 // arduino pin wired to LCD RS
#define pin_EN 9 // arduino pin wired to LCD EN
#define pin_d4 2 // arduino pin wired to LCD d4
#define pin_d5 5 // arduino pin wired to LCD d5
#define pin_d6 6 // arduino pin wired to LCD d7
#define pin_d7 7 // arduino pin wired to LCD d8
#define pin_BL 3 // arduino pin wired to LCD backlight dimmer
#define dimmerValue 120 // medium level

#define blinkConstant 2400 // milliseconds

// Start the LCD display library
  LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);
  // LiquidCrystal lcd( 8,  9,  2,  5,  6,  7);

/*-----( Declare objects )-----*/
// Set up an oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass address of our oneWire instance to Dallas Temperature. 
DallasTemperature sensors(&oneWire);


/*-----( Declare Variables )-----*/

 // My devices
DeviceAddress Probe01 = { 0x28, 0x10, 0x1C, 0xFE, 0x08, 0x00, 0x00, 0x59 }; // WHITE
DeviceAddress Probe02 = { 0x28, 0xBC, 0x5A, 0xFF, 0x08, 0x00, 0x00, 0x82 }; // YELLOW
DeviceAddress Probe03 = { 0x28, 0x79, 0xEE, 0xFE, 0x08, 0x00, 0x00, 0x29 }; // RED

// Temperature resolution: fast=9, most_precise=12. Choose 9..12
int precisionBits = 12;

// --------------------------------------------------------------------

void setup()   /****** SETUP: RUNS ONCE ******/
{
//------- Initialize the Temperature measurement library--------------
  sensors.begin();
  // set the resolution to 10 bit (Can be 9 to 12 bits .. lower is faster)
  sensors.setResolution(Probe01, precisionBits);
  sensors.setResolution(Probe02, precisionBits);
  sensors.setResolution(Probe03, precisionBits);


//---------------- Initialize the lcd ------------------  
  lcd.begin (16,2);  // 20 characters, 4 lines
// Dim on the backlight
  analogWrite(pin_BL,dimmerValue); // this is oldschool; lcd.backlight() not available.

}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  sensors.requestTemperatures(); // Send the command to get temperatures

  lcd.clear();  // Reset the display  
  // lcd.home(); Takes time and does almost nothing

  // Print the values on the LCD, home=0,0
  
  lcd.setCursor(8,1);    //Start at character 8 on line 1
  lcd.print("Celsius"); // show it FIRST so that it will not blink
  
  lcd.setCursor(0,0); //Start at character 0 on line 0
  lcd.print("=");
  displayTemperature(Probe01);  
  
  lcd.setCursor(8,0); //Start at character 8 on line 0
  lcd.print("=");
  displayTemperature(Probe02);  
  
  lcd.setCursor(0,1); //Start at character 0 on line 1
  lcd.print("=");
  displayTemperature(Probe03);  
  
  delay(blinkConstant);

  /* Display planning - with certain compromises, it is possible
  to put 4 sensor's results onto the 16x2 display.
  The limitation is - we do not plan for >99.99 degree values.
  nor plan we for - (minus) values. We only use Celsius degrees.
  Outside of these considerations, a 16x2 display isn't sufficient.
  */
}//--(end main loop )---

/*-----( Declare User-written Functions )-----*/
void displayTemperature(DeviceAddress deviceAddress)
{

float tempC = sensors.getTempC(deviceAddress);

   if (tempC == -127.00) // Measurement failed or no device found
   {
    lcd.print("Error");
   } 
   else
   {

   lcd.print(tempC);
   lcd.print((char)223); // http://forum.arduino.cc/index.php?topic=19002.0
   /* No imperialism! 
   lcd.print("C");
   lcd.print(" F=");
   lcd.print(DallasTemperature::toFahrenheit(tempC)); // Convert to F
   */
   }
}// End printTemperature

//*********( THE END )***********


