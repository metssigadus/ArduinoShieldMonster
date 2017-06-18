//!
//! @file    TestSysLogger.ino
//! @author  Identity Withheld <metssigadus@xyz.ee> (l)
//! @date    2017-06-17
//! @brief   A minimal syslog sketch
//!          Derived from "Alarm sketch" by Ryan Adams 2012-02-29
//!          Orig src: https://forum.arduino.cc/index.php?topic=89936.0
//! @note    Reformatted but not yet tested
//! @note    This program is to stress test your hardware and syslog
//! @details
//!    <b>Last Modified:         </b> 2017-06-17                   \n
//!    <b>Version:               </b> 0.0.3                        \n
//!    <b>Company:               </b> R777                         \n
//!    <b>Project:               </b> ArduinoShieldMonster         \n
//!    <b>Language:              </b> nugged Arduino C/C++         \n
//!    <b>Compiler:              </b> Arduino IDE 2:1.0.5-dfsg2-4  \n
//!    <b>uC Model:              </b> Arduino Uno                  \n
//!    <b>Computer Architecture: </b> ATMEGA                       \n
//!    <b>License:               </b> bits tend to be free         \n
//!    <b>Documentation Format:  </b> emulating Eclipse by vi      \n
//!
//!  An Ethernet Shield is required.
//!  A reasonably well configured syslog server is expected
//!     e.g. one with /var/log/<IP>/Year/Month/Date/facility.log structure.


//==========================================================================//
//================================ INCLUDES ================================//
//==========================================================================//

#include <SPI.h>          // We need an Ethernet Sheild and that uses SPI
#include <Ethernet.h>
#include <EthernetUdp.h>


//==========================================================================//
//================================= DEFINES ================================//
//==========================================================================//

static const char theNameOfThisApp[11] = "TestLogger";  // as it will show up in syslog files
static const char currentVersion[16] = "2017-06-18T0601"; // We might need to display it

/* The MAC address that our Ethernet Shield should use.
 	Well tuned DHCP servers give us a static lease according the MAC
 */
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

/* Don't misuse anybody else's syslog server
   and be sure it is actually configured to listen
   to these nasty improvised little IoT thingies
   (port, protocol, facility, priority et c)
 */
IPAddress syslogServer(192, 168, 2, 52); 

/* Some people claim #define's are bad
   due to beginners won't note semicolons and equal signs;
   Me like #define's because the preprocessor will adjust types
   which I am unaware of ;)
   */
#define localUdpPort     51400 // local UDP port to send from
#define remoteSyslogPort 51400  // the UDP port syslog server listens to (normally 514)

const int maxSyslogMsgLength = 95; // The maximum length of the transmitted syslog message

/* In case of glitches, to avoid SPI problems
   you might to disable the SD card schemistry on the Ethernet Shield,
        pinMode(4,OUTPUT);
        digitalWrite(4, HIGH);
 */


//==========================================================================//
//================================ VARIABLES ===============================//
//==========================================================================//

 // Avoiding stack/heap kaboom thus so explicit
const char message01[28] = "Ethernet has been started. ";
const char message02[47] = "FATAL: Failed to configure Ethernet via DHCP. ";
const char message03[22] = "My code is dated as: ";
const char message04[16] = "My DHCP IP is: ";
const char message05[22] = "BOOT was suxxessful. ";
const char message06[23] = "Next loop; iteration #";
const char message07[43] = "We log to syslog, thus mostly nothing here";
const char message08[23] = "UDP library was bound.";
const char message09[13] = "; RAM free: ";
const char message10[7]  = " Bytes";
const char message11[26] = "; runtime milliseconds: ";
//                         0    v    !    v    !    v    !    v    !    v 

// Iteration counter
 unsigned int iteration = 1; // Counting how many times we entered the loop()
 
  char myBuff[10]; // 4,294,967,295 (2^32 - 1) vs 65,535 (2^16) - 1
  char numBuff[10]; // 4,294,967,295 (2^32 - 1) vs 65,535 (2^16) - 1
  char msgBuff[maxSyslogMsgLength] = ""; // a buffer of that long

 // Derive an EthernetUDP instance
EthernetUDP Udp;


//==========================================================================//
//================================= SETUP ==================================//
//==========================================================================//

//! @brief    Setup function (a.k.a. init)
void setup()
{
  Serial.begin(9600);
  Serial.print("#### ");
  Serial.println(message07);
  Serial.println("");

  // start Ethernet
  if (Ethernet.begin(mac) == 0) {
    Serial.println(message02);
    // no point in carrying on, so do nothing forevermore:
    for (;;)
      ;
  }
  delay(1001); // Wait 1 sec for EthernetShield raising up
  Udp.begin(localUdpPort); // Lazy we should actually check the return code
  Serial.println(message08);
  Serial.println(message01);
  Serial.print(message04);
  Serial.println(Ethernet.localIP());
  
  sendSyslogMessage(2, message05); // Send our very first syslog msg

/*   String message = "My DHCP address is: ";
  message = message + MyAddress(Ethernet.localIP());
  sendSyslogMessage(5, message);
*/

  strcat (msgBuff, message04);
  strcat (msgBuff, ip2CharArray(Ethernet.localIP()) );
  sendSyslogMessage(5, msgBuff);

  strcpy (msgBuff, message03);
  strcat (msgBuff, currentVersion);
  sendSyslogMessage(1, msgBuff);
  

}  // End of SETUP

//==========================================================================//
//================================== LOOP ==================================//
//==========================================================================//


void loop()
{
    //! @debug
  Serial.print(message06);
  Serial.println (iteration);
  
  strcpy (msgBuff, ""); // Cleaning?
  sprintf(numBuff, "%5u", iteration);
  strcpy (msgBuff, message06);
  strcat (msgBuff, numBuff);
  strcat (msgBuff, message11);
  // src for sprintf  %10ld trick: http://forum.arduino.cc/index.php?topic=95175.0
  sprintf(numBuff, "%10ld", millis());
  strcat (msgBuff, numBuff);
  strcat (msgBuff, message09);
  sprintf(numBuff, "%5u", freeRam());
  strcat (msgBuff, numBuff);
  strcat (msgBuff, message10);
  sendSyslogMessage(1, msgBuff); // This time on facility no 1
     
  delay(10000); // approx 1 min
  iteration++;
    /* Some ppl recommend Ethernet.maintain() here, we won't do this for 2 reasons:
   	a) a too frequent loop, network admins will complain seeing it it every 60 sec
   	b) in case of a static lease, nobody else can grab our IP
   		(caveat emptor: there are secure installations where
   		the FW stops to bypass packets for an expired lease)
   Ethernet.maintain(); 
   */

}


//==========================================================================//
//=============================== FUNCTIONS ================================//
//==========================================================================//


void sendSyslogMessage(int severity, String textToTransfer)
{
  /*
   0 Emergency: system is unusable
   1 Alert: action must be taken immediately
   2 Critical: critical conditions
   3 Error: error conditions
   4 Warning: warning conditions
   5 Notice: normal but significant condition
   6 Informational: informational messages
   */

  /* TODO: on REAL systems the facility concept is
   	even more important than severity b/c
   	messages are routed into different log files
   	based on this value.
   Need to code the facility array and 8x math.
   Currently 32 goes to -> auth.log on my particular syslog ;)
   */

  // Some black magic building up the UDP packet payload
  byte pri = (32 + severity);
  String priString = String(pri, DEC);
  String buffer = "<" + priString + ">" + theNameOfThisApp + " " + textToTransfer;
  int bufferLength = buffer.length();
  char char1[bufferLength+1];
  for(int i=0; i<bufferLength; i++)
  {
    char1[i]=buffer.charAt(i);
  }

  char1[bufferLength] = '\0';
  Udp.beginPacket(syslogServer, remoteSyslogPort);
  Udp.write(char1);
  Udp.endPacket(); //this is slow 

}

//-------------------------------

//! @brief    Shows how many bytes of free memory left
//! @details  Some black magic far above the beginners' level
//! @note   Src: https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory

int freeRam ()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}


//------------------------------------------
//! @brief    Obtain the IP address of the Ethernet Shield
//! @details  We are dependent on which IP the DHCP server actually gave us
//! @note     Src: https://stackoverflow.com/questions/28119653/arduino-display-ethernet-localip
char* ip2CharArray(IPAddress ip) {
  static char a[16];
  sprintf(a, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  return a;
}

// EOF

