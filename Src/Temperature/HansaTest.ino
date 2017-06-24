//!
//! @file    HansaTest.ino
//! @author  Identity Withheld <metssigadus@xyz.ee> (l)
//! @date    2017-06-19
//! @brief   A mix of I2C and DS18B20 related sketches to test my particular HW
//! @note    Not yet finished, this is an initial draft for something else.
//!          I never understood why some popular sketches force people to
//!          manually write down the DS18B20 addresses ... this is crazy.
//!          Automatic discovery is possible.
//!
//! @details
//!    <b>Last Modified:         </b> 2017-06-24                   \n
//!    <b>Version:               </b> 0.0.6                        \n
//!    <b>Company:               </b> R777                         \n
//!    <b>Project:               </b> ArduinoShieldMonster         \n
//!    <b>Language:              </b> C++                          \n
//!    <b>Compiler:              </b> Arduino IDE 2:1.0.5-dfsg2-4  \n
//!    <b>uC Model:              </b> Arduino Uno                  \n
//!    <b>Computer Architecture: </b> ATMEGA                       \n
//!    <b>License:               </b> bits tend to be free         \n
//!    <b>Documentation Format:  </b> Emulating Eclipse by vi      \n
//!
//!  This sketch is expected to run on my ArduinoShieldMonster
//!     
//!   /^^^^^\
//!  | o o o | DS18b20
//!  +-------+
//!    3 2 1

/* Yeah guys you ask why HANSA- ? It has something to do with Hanseatic League
   Because of some 500+ domestic companies had that prefix in name.
   Thus: hansa- is a prefix as good as those: giga- mega- super- hyper- ;)
 */
 
//==========================================================================//
//================================ INCLUDES ================================//
//==========================================================================//
 
#include <Wire.h>
#include <OneWire.h>

//==========================================================================//
//================================ DEFINES =================================//
//==========================================================================//

#define SENSOR_PIN A2  // For 1-wire bus: any pin 2 to 12 (not 13) and A0 to A5
// Very naturally, I2C is located at SDA=18=A4, SCL=19=A5

/* --- LCD pin assignments
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

*/

//==========================================================================//
//================================ VARIABLES ===============================//
//==========================================================================//

 const char version[] = "2017-06-24T2236"; // For cases we need to show or syslog this 

 const static String i2cDevice80 = " 24LC512 Serial EEPROM; addr=000";
 const static String i2cDevice81 = " 24LC512 Serial EEPROM; addr=001";
   // A sole device of this type allowed on bus - its I2C address cannot be modified 
 const static String i2cDevice104 = "Dallas DS3231 RTC & temperature sensor";

/* NB! src: http://www.esp8266.com/viewtopic.php?f=32&t=12623
   All the 24C02/04/08/16 chips are not following the same addressing scheme
   than bigger chips such 24C64. It is normal that 24C08 respond four times
   to scanner since it provide 4 page of 256 bytes. The library you are using
   probably doesn't handle those small chips because it is providing 16 bits
   addressing instead of 8 bits. I would then suggest to unsolder this 24C16
   and throw it away and solder a bigger one such 24C64 or even bigger... 
*/
   // Those are manageable by serial numbers lasered into
 const static String wireDevice040 = "Dallas DS18B20 temperature sensor";
 const static String wireDevice000 = "UNKNOWN DEVICE";
 
 const int maxOnewireDevices = 5; // number of planned OneWire temperature sensors
 
   // A global 2D array of <maxOnewireDevices> rows of 8-byte addresses
 byte inventory[maxOnewireDevices][8]; // here we store DS18b20 addresses
 int countOfI2CDevices = 0;     // we count these later
 int countOfOneWireDevices = 0; //    and these too
 
 OneWire  ourBus(SENSOR_PIN);   // Derive a 1-wire object to work with


//==========================================================================//
//================================= SETUP ==================================//
//==========================================================================//

//! @brief    Setup (init) function
void setup() {
  Serial.begin (115200);
  // Leonardo special: wait for serial port to connect
  while (!Serial) 
    {
    }

   // Preparatory flow
  Serial.println ("#### Discovery started:\n\r");
  Serial.println ();
  
    discoverI2CDevices(); //  FUNCTION: Scanning I2C bus
    discoverOneWireDevices();  //  FUNCTION: Enumerating devices on 1-wire bus

  // Inventory done. Reporting
  Serial.print ("    === TOTAL Discovered: ");
  Serial.print ((countOfI2CDevices+countOfOneWireDevices), DEC);
  Serial.println (" interesting device(s).");
  
  Serial.print ("           I2C device(s): ");
  Serial.println (countOfI2CDevices, DEC);

  Serial.print ("        1-wire device(s): ");
  Serial.println (countOfOneWireDevices, DEC);
  
  Serial.println ("#### Discovery finished.\n\r\n\r");

  // Then we bring up the LCD display and show nice intros on it :)
  
  // This is the place to initialize the Ethernet Shield 
  // and right after that - syslog

  // In case we discovered a RTC chip and Ethernet is already up,
  //    then renew the time
 
  // By a separate function, reflect the inventory to syslog.
   sendInventoryToSyslog(inventory);
 
}  // end of setup

//==========================================================================//
//================================== LOOP ==================================//
//==========================================================================//

//! @brief    Main loop
void loop() 
{

    /* .plan: We take the addresses of discovered devices from an array,
    we also consider the temp sensor within RTC clock
    What we do with the current temps:
           a) send these to syslog
           b) send these onto LCD
           c) might be, later, record these on SD or in I2C EEPROM
    We shall implement a logic that we only send temps to syslog when
    a temp value differs from the previous measurement or time has passed,
    */

  delay(1001);
  
  // Ethernet.maintain(); How to use it sensibly over a time, not every minute?
  

} // END of LOOP


//==========================================================================//
//=============================== FUNCTIONS ================================//
//==========================================================================//


//----------------------------------------------------
//! @brief    Reports I2C devices discovered on the bus
//! @note     Derived: I2Cscanner src: Nick Gammon 2011-04-20
//! @details  not yet finished. Only shows device data, could record it.
//!           
void discoverI2CDevices(void) {

  Serial.println ("======== Scanning I2C bus for all possible addresses");
  byte count = 0;
  
  Wire.begin();
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
      {
      Serial.print ("A device responded to address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.print (") ");
        // switch:
      if ( i == 104) {
      Serial.print ("- a " + i2cDevice104);
      }
      if ( i == 80) {
      Serial.print ("- a " + i2cDevice80);
      }
      if ( i == 81) {
      Serial.print ("- a " + i2cDevice81);
      }
      
      Serial.println();
      
      count++;
      countOfI2CDevices++;
      } // end of if, no else.
      
  } // end of for
  Serial.println ("======== Done with I2C bus.");
  Serial.print ("    Discovered: ");
  Serial.print (count, DEC);
  Serial.println (" device(s).");
  Serial.println();

}

//----------------------------------------------------
//! @brief    A scanner for Arduino  1-wire bus, data goes to an inventory array
//! @details   very loosely based on terry@yourduino.com V1.1 2013-01-17 sketch
//! @note     src: http://arduino-info.wikispaces.com/Brick-Temperature-DS18B20#mult

void discoverOneWireDevices(void) {
  int i = 0; //rows:  one row per discovered device
  int k; // columns: eight address bytes for each discovered device 

  Serial.print("======== Enumerating 1-Wire devices...\n\r");// "\n\r" is NewLine 
  
     /* As long as there are devices on the bus,
        the he onewire lib returns a 8-byte address for the next device */
     
  while(ourBus.search(inventory[i])) {  
    
    Serial.print(" +"); // a plus sign printed out for every discovered sensor
    i++ ; // next sensor, next row
    countOfOneWireDevices = i; // Well, so far...
       // Dammit! While(instance+1) is leading to memory corruption
       // Elegant but inflexible. 
    if (countOfOneWireDevices == maxOnewireDevices) {
      Serial.println("\n\r        Notice: maximum configured capacity reached: >= const maxOnewireDevices.");
      Serial.println("        If ANY more devices (plus signs) appear beyond this point, the memory *WILL* be corrupted afterwards.");
    }
  }  // end of WHILE
  
  ourBus.reset_search(); // 'nuff to snoop

  Serial.println();
  Serial.println("======== Done scanning OneWire.");

  Serial.print ("    Discovered: ");
  Serial.print (i, DEC);
  Serial.println (" device(s).");
  Serial.println();
    
  return;
} // end of the func


/*
WHITE:  0x28, 0x10, 0x1C, 0xFE, 0x08, 0x00, 0x00, 0x59
YELLOW: 0x28, 0xBC, 0x5A, 0xFF, 0x08, 0x00, 0x00, 0x82
RED:    0x28, 0x79, 0xEE, 0xFE, 0x08, 0x00, 0x00, 0x29
TRANSP: 0x28, 0xFF, 0x8B, 0x45, 0x88, 0x16, 0x03, 0x71
BLUE:   0x28, 0xFF, 0x68, 0x45, 0x91, 0x16, 0x04, 0xA2
*/

//----------------------------------------------------
//! @brief    Arduino 1-wire bus inventory printout from an array
//! @details  Very loosely based on terry@yourduino.com V1.1 2013-01-17
//! @note     src: http://arduino-info.wikispaces.com/Brick-Temperature-DS18B20#mult

void sendInventoryToSyslog(byte inventory[][8]) {
  int i = 0; //rows:  one row per discovered device
  int k; // columns: eight address bytes for each discovered device 

    Serial.println("#### sendInventory() started.");
    for ( i = 0; i < countOfOneWireDevices ; i++) {
 
    Serial.print("\n\r\'1-Wire\' device registered: ");
              if (inventory[i][0] == 40) {
                 Serial.print(wireDevice040); // recognizing Dallas 18B20 ;)
               }
              else {
                 Serial.print(wireDevice000); // a safe default
              }
    Serial.print("; addr: ");
    
    for( k = 0; k < 8; k++) {
       
      Serial.print("0x");
      if (inventory[i][k] < 16) {
        Serial.print('0'); // leading zero
      }
      Serial.print(inventory[i][k], HEX);
      if (k < 7) {
        Serial.print(", ");
      }
    }  // end of FOR K
    
      if ( OneWire::crc8( inventory[i], 7) != inventory[i][7]) {
      Serial.print(" PANIC! CRC is not valid!\n\rAborting the enumeration\n\r");
      return;
      }
      
    }  // end of FOR I

    Serial.println("\n\r\n\r#### DONE with sendInventory()");
} // end of FUNC

// EOF

