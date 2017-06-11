/* YourDuino Multiple DS18B20 Temperature Sensors on 1 wire
  Connections:
  DS18B20 Pinout (Left to Right, pins down, flat side toward you)
  - Left   = Ground
  - Center = Signal (Pin 2):  (with 3.3K to 4.7K resistor to +5 or 3.3 )
  - Right  = +5 or +3.3 V

   Questions: terry@yourduino.com 
   V1.01  01/17/2013 ...based on examples from Rik Kretzinger
   
/*-----( Import needed libraries )-----*/
// Get 1-wire Library here: http://www.pjrc.com/teensy/td_libs_OneWire.html
#include <OneWire.h>

//Get DallasTemperature Library here:  http://milesburton.com/Main_Page?title=Dallas_Temperature_Control_Library
#include <DallasTemperature.h>

String versionNo = "2017-06-11T2355";

/*-----( Declare Constants and Pin Numbers )-----*/
#define ONE_WIRE_BUS_PIN A2

/*-----( Declare objects )-----*/
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS_PIN);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

/*-----( Declare Variables )-----*/
// Assign the addresses of your 1-Wire temp sensors.
// See the tutorial on how to obtain these addresses:
// http://www.hacktronics.com/Tutorials/arduino-1-wire-address-finder.html

DeviceAddress Probe01 = { 0x28, 0x10, 0x1C, 0xFE, 0x08, 0x00, 0x00, 0x59 }; // WHITE
DeviceAddress Probe02 = { 0x28, 0xBC, 0x5A, 0xFF, 0x08, 0x00, 0x00, 0x82 }; // YELLOW
DeviceAddress Probe03 = { 0x28, 0x79, 0xEE, 0xFE, 0x08, 0x00, 0x00, 0x29 }; // RED

// Temperature resolution: fast=9, most_precise=12. Choose 9..12
int precisionBits = 12;



void setup()   /****** SETUP: RUNS ONCE ******/
{
  // start serial port to show results
  Serial.begin(9600);
  Serial.print("Initializing Temperature Control Library Version ");
  Serial.println(DALLASTEMPLIBVERSION);
  
  // Initialize the Temperature measurement library
  sensors.begin();
  
  // set the resolution to 10 bit (Can be 9 to 12 bits .. lower is faster)
  sensors.setResolution(Probe01, precisionBits);
  sensors.setResolution(Probe02, precisionBits);
  sensors.setResolution(Probe03, precisionBits);

}//--(end setup )---

void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  delay(1000);
  Serial.println();
  Serial.print("Number of Devices registered with code = ");  
  Serial.println(sensors.getDeviceCount());   
  Serial.print("Precision of measurements set as: ");  
  Serial.print(precisionBits); 
  Serial.println(" bits."); 
  Serial.print("Getting temperatures... ");  
  Serial.println();   
  
  // Command all devices on bus to read temperature  
  sensors.requestTemperatures();  
  
  Serial.print("Probe 01 -  WHITE: temperature is:   ");
  printTemperature(Probe01);
  Serial.println();

  Serial.print("Probe 02 - YELLOW: temperature is:   ");
  printTemperature(Probe02);
  Serial.println();
 
  Serial.print("Probe 03 -    RED: temperature is:   ");
  printTemperature(Probe03);
  Serial.println();
   

   
  
}//--(end main loop )---

/*-----( Declare User-written Functions )-----*/
void printTemperature(DeviceAddress deviceAddress)
{

float tempC = sensors.getTempC(deviceAddress);

   if (tempC == -127.00) 
   {
   Serial.print("Error getting temperature  ");
   } 
   else
   {
   Serial.print("C: ");
   Serial.print(tempC);
   // Phuck imperators! Serial.print(" F: ");
   // Phuck imperators! Serial.print(DallasTemperature::toFahrenheit(tempC));
   }
}// End printTemperature
//*********( THE END )***********

