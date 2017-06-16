/*
 * a minimal syslog sketch
 * derived from "Alarm sketch" by Ryan Adams 2/29/2012
 * #define sketchVersion "0.1.5"
 * Src: https://forum.arduino.cc/index.php?topic=89936.0
 */

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define localUdpPort 51400
#define syslogPort 51400

String versionNo = "2017-06-16T1329";
unsigned int iteration = 1;
char myBuff[10]; // 4,294,967,295 (2^32 - 1) vs 65,535 (2^16) - 1

/* Yet no need to disable SD card to avoid SPI problems
  pinMode(4,OUTPUT);
  digitalWrite(4, HIGH); 
*/

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress syslogServer(192, 168, 2, 52); 

// Derive an EthernetUDP instance
EthernetUDP Udp;

void setup()
{
  Serial.begin(9600);
  Serial.println("We log to syslog, thus mostly nothing here");

  // start Ethernet
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for (;;)
      ;
  }
  delay(1000); //Wait 1 sec for Ethernet
  Udp.begin(localUdpPort);

  Serial.println("Ethernet has been started");
  Serial.print("My IP is: ");
  Serial.println(Ethernet.localIP());
  
  sendSyslogMessage(2, "Suxxessful BOOT.");
  String message = "My DHCP address is: ";
  message = message + MyAddress(Ethernet.localIP());
  sendSyslogMessage(5, message);

}

void loop()
{
    String message = "Go next loop, iteration #";
    sprintf(myBuff, "%5u", iteration); // sprintf() overhead about 1582 bytes of memory
    message = message + myBuff;
    message = message + "; runtime milliseconds: ";
    sprintf(myBuff, "%10ld", millis()); // http://forum.arduino.cc/index.php?topic=95175.0
    message = message + myBuff;
    message = message + "; RAM free: ";
    sprintf(myBuff, "%5u", freeRam());
    message = message + myBuff;
    message = message + " bytes";
    sendSyslogMessage(1, message);
    // sendSyslogMessage(6, "Heartbeat Signal");
    delay(60000); // approx 1 min
    iteration++;

}


void sendSyslogMessage(int severity, String message)
{
  /*
   0 Emergency: system is unusable 
   1 Alert: action must be taken immediately 
   2 Critical: critical conditions 
   3 Error: error conditions 
   4 Warning: warning conditions 
   5 Notice: normal but significant condition 
   6 Informational: informational messages 
   
   Only severity levels 0, 1, and 2 will trigger an email alert
   */

  // TODO: on REAL systems the facility concept is extremely important
  //       for logs routing. 32 -> auth ;)
  byte pri = (32 + severity);
  String priString = String(pri, DEC);
  String buffer = "<" + priString + ">" + "TestLogger " + message;
  int bufferLength = buffer.length();
  char char1[bufferLength+1];
  for(int i=0; i<bufferLength; i++)
  {
    char1[i]=buffer.charAt(i);
  }

  char1[bufferLength] = '\0';
  Udp.beginPacket(syslogServer, syslogPort);
  Udp.write(char1);
  Udp.endPacket(); //this is slow 

}

//-------------------------------

// https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory
    int freeRam () 
    {
      extern int __heap_start, *__brkval; 
      int v; 
      return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
    }

//-------------------------------
  String MyAddress(IPAddress address) // https://stackoverflow.com/questions/28119653/arduino-display-ethernet-localip
{
 return String(address[0]) + "." + 
        String(address[1]) + "." + 
        String(address[2]) + "." + 
        String(address[3]);
}


