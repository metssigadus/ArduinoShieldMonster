# ArduinoShieldMonster
Programs for my custom HW-modded Arduino Shields. Everything derived from public domain, no professional value. I keep these modified Arduino sketches for my own Monster Shield.

It is a known problem that certain Arduino shields (http://shieldlist.org/) are mutually exclusive.
In most cases the cause is very simple - shields are using overlapping resources.

![Arduino Shield Monster](Pic/20170610_225047-1c.jpg?raw=true "Arduino Shield Monster")

E.g. a conflict will arise between Ethernet and LCD Keypad Shields, because both use pin 10.

If physical modding is OK four you, this is the way to go. One should not forget, physical mods are later to be reflected in code.

To test certain concepts and practice Arduinos, I built a "monstershield" Arduino. Well, Internet is full of real monsters, my only interest was to put together EtherShield and LCD Keypad Shield.

- Beneath of anything is Arduino Uno R3.
- Then comes the Ethernet Shield. I have tried the "normal" Wiznet 5100 R3 shield and also the one with marking "MEGA compatible" that has more LEDs onboard and, which is very important, has the interrupt line disconnect option on the board.
- Then comes a (mechanically modified) ScrewShield v1 from Iteaduino - it helps debugging and raises LCD Shield over the Ethernet jack.
- On the top of everything sits the LCD Keypad Shield that has been HW modified.

This is my pin plan:

![Pin Plan](Pic/Planning.png?raw=true "Avoiding Pin Conflicts")

The success so far:

- Arduino Uno struggles to provide power. There are glitches sometimes. An external PSU should be better ;). 
- mechanical rework for LCD was successful. LCDKeypadCheck says: BL Cirquit Good, see http://forum.arduino.cc//index.php?topic=96747 .
- UdpNtpClient works well.
- EtherLCDserver works well
- tried to implement Syslog by arcao https://github.com/arcao/Syslog but did not succeed - the sketch hangs right after first syslog.log call. Had to use a much more simpler hack.

While people were at Rammstein concert (2017-06-11), I soldered the Sensor Board:

![Sensor Board](Pic/20170611_232448-a.jpg?raw=true "Screw Shield 1.0 with sensors")

This is a screenshot of earlier WebServer_004.ino (In September 2016 I polished it 6 hrs up to trying to create the art deco table with round corners ... then I understood it's time to finish.)

![WebServer_004](Pic/WebServer_004--2016-06-11.png?raw=true "A refactored version of the chrestomathic Analog Webserver")


