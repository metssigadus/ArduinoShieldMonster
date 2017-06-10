/*
  Web Server
 
 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)
 
 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 
 */

// ========================
// #define needConsoleDebug 
// Define it while debugging
// But do consider - then the web srv won't start until the CONSOLE will be opened
// ========================

// ToDo DS2401: https://forum.sparkfun.com/viewtopic.php?f=32&t=34208
// ToDo Ethernet Addr from smth unique, e.g. DS4201: https://github.com/ArthurGuy/Arduino_SerialNumber
String versionNo = "2016-09-09T0300";

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0F, 0xBB, 0xAE };
IPAddress ip(192,168,2,166);

// the dns server ip
IPAddress dnServer(8, 8, 8, 8);
// the router's gateway address:
IPAddress gateway(192, 168, 2, 1);
// the subnet:
IPAddress subnet(255, 255, 255, 0);

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);

// Initialize Request Counter
long sitaxPalju = 0L;


void setup() {
  #ifdef needConsoleDebug
 // Open serial communications and wait for port to open:
      Serial.begin(9600);
        while (!Serial) {
        ; // wait for serial port to connect. Needed for Leonardo only
      }
    #else
        ;
    #endif

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip, dnServer, gateway, subnet);
  server.begin();
  #ifdef needConsoleDebug
      Serial.print("server is at ");
      Serial.println(Ethernet.localIP());
  #endif
}

// https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory
    int freeRam () 
    {
      extern int __heap_start, *__brkval; 
      int v; 
      return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
    }





void loop() {

  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    sitaxPalju++;
    
  #ifdef needConsoleDebug
      Serial.println("new client");
  #endif
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        #ifdef needConsoleDebug
        Serial.write(c);
        #endif
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
	  client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          
          // Here the HTML doc starts
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<head>");
          client.println("<title>");
          client.println("Arduino Mini Web Server - main page");
          client.println("</title>");
          client.println("<meta charset='UTF-8'>");
          client.println("</head>");
          client.println("<body bgcolor='#E6E6FA'>");
          
          // Generic information
          client.print("<B>Server IP:</B> ");
          client.println(Ethernet.localIP());
          client.print("<BR>");
          client.print("<B>Free SRAM:</B> ");
          client.print(freeRam());
          client.println(" bytes");
          client.println("<BR>");
          client.print("<B>WWW Requests after last restart:</B> ");
          client.print(sitaxPalju);
          client.println("");
          client.println("<BR>");
          client.println("<HR>");
          // ToDO: https://www.arduino.cc/en/Tutorial/UdpNtpClient
          
          // Forming a nice old-school table
          client.println("<table border='1'>");  
          client.print("<tr bgcolor='#AAAAAA'>");
          client.print("<th>AnalogChannel</th>");
          client.print("<th>SensorReading</th>");
          client.println("</tr>");
          // ideas also from: https://www.arduino.cc/en/Tutorial/ReadAnalogVoltage

          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            float voltage= sensorReading * (5.0 / 1023.0);
            client.print("<tr bgcolor='#CCCCCC'>");
            client.print("<td align='center'>");
            client.print(analogChannel);
            client.print("</td>");
            client.print("<td align='center'>");
            client.print(voltage);
            client.print(" V");
            client.print("</td>");
            client.println("</tr>");       
          }
          
          // Closing the table and everything
          client.print("</table>");
          client.println("</body>");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    #ifdef needConsoleDebug
        Serial.println("client disonnected");
    #endif
  }
}

// Extra: http://stackoverflow.com/questions/13960902/get-client-ip-address-with-arduino

