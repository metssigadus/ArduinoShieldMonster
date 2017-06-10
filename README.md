# ArduinoShieldMonster
Programs for my custom HW-modded Arduino Shields. Everything derived from public domain, no professional value. I keep these modified Arduino sketches for my own Monster Shield.

It is a known problem that certain Arduino shields (http://shieldlist.org/) are mutually exclusive.
In most cases the cause is very simple - shields are using overlapping resources.

![Arduino Shield Monster](Pic/20170610_225047-1c.jpg?raw=true "Arduino Shield Monster")

E.g. a conflict will arise between Ethernet and LCD Keypad Shields, because both use pin 10.

If physical modding is OK four you, this is the way to go. One should not forget, physical mods are later to be reflected in code.

To test certain concepts and practice Arduinos, I built a "monstershield" Arduino. Well, Internet is full of real monsters, my only interest was to put together EtherShield and LCD Kepypad Shield.

- Beneath of anything is Arduino Uno R3.
- Then comes the Ethernet Shield. I have tried the "normal" Wiznet 5100 R3shield and also the one with marking "MEGA compatible" that has more LEDs onboard.
- Then comes a (mechanically modified) ScrewShield v1 from Iteaduino - it helps debugging and raises LCD Shield over the Ethernet jack.
- On the top of everything is LCD Keypad Shield that has been HW modified (Reset button joined to ladder, dimmer upgradedand relocated from digital pin 10 to 03), while analog input relocated from PIN A0 (14) to A3 (17). Last but not least, DB4 of LCD was relocated away from Pin04 to permit experiments with the onboard SD card.

This is my pin plan:

![Pin Plan](Pic/Planning.png?raw=true "Avoiding Pin Conflicts")

The success so far:

- Arduino Uno struggles to provide power. There are glitches sometimes. An external PSU should be better ;). 
- mechanical rework for LCD was successful. LCDKeypadCheck says: BL Cirquit Good, see http://forum.arduino.cc//index.php?topic=96747 .The most critical operation was soldering off the LCD subassembly. I did it partially mechanically - sawing through the 10 pins and desoldering the rest six pins. Very naturally, the pin headers for the LCD were to be substituted.
- UdpNtpClient works well.
- EtherLCDserver works well
- tried to implement Syslog by arcao https://github.com/arcao/Syslog but did not succeed - the sketch hangs right after first syslog.log call. Will return to that later.
