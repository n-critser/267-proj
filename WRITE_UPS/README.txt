                      README.org for project-267
                      ==========================

Author: n-critser <nc_chaos@localhost>
Date: 2014-02-04


Table of Contents
=================
1 Abstract 
2 Components 
    2.1 Speakers 
    2.2 Arduino Uno - 2 boards (1 for speech , 1 for everything else) 
    2.3 Battery pack/ ON-Off 
    2.4 4-3*3 led matrices for simulated lip movement 
    2.5 shift registers 
3 TASKS 
    3.1 1st iteration Makefile 
    3.2 Chris Kuethe tutorial seperate dir 
    3.3 Serial input from keyboard 
    3.4 Serial 2 way commune 
    3.5 Text to speach testing for arduino 
    3.6 ?   Using a SD card for storage 
    3.7 Using libraries in C 
    3.8 Accessing storage in C 
    3.9 Parts list for "project" 
4 SERIAL 
    4.1 Output 
    4.2 Input 
5 Compile-Pipeline 
    5.1 Command Line build-&-upload 
        5.1.1 Example: 
    5.2 Makefile build-&-upload 
6 Bugs-&-Fixes 
    6.1 1 
    6.2 2 
        6.2.1 ERROR: warning: #warning "F_CPU not defined for <util/delay.h>" [-Wcpp] 
        6.2.2 FIX:  put the define in FILE.c itself. 
7 c-arduino basics 
8 Tool-Chain 
    8.1 sudo apt-get install make avr-libc avrdude binutils-avr gcc-avr gdb-avr 
9 Tutorials 
    9.1 German robot course 
    9.2 Amercan_Tutorials 
        9.2.1 Chris Kuethe--https://www.mainframe.cx/~ckuethe/avr-c-tutorial/ 
10 Arduino-Uno 
11 AVR-info 
12 Links 
13 AtmEGA328-datasheet++ 
    13.1 Pins & Ports 
        13.1.1 Ports 
    13.2 Qtouch capacitive touch sensing 
    13.3 Program on the fly SPI 


1 Abstract 
~~~~~~~~~~~
  My goal is to create an talking entity using the Arduino Uno rev3 and 
some speakers all contained within an anthropomorphic representation. 
I aim to create a cool project for 267 class that forces me to learn the inner 
workings of the atmega328 microcontroller.  This may include learning the 
low level assembly instructions used by the processor itself, but is not 
limited to this level of programming.

   I will be using a c-library which pushes phoneme simulations and the
code necessary to interact with the serial read and write registers of 
the dedicated speech board.  

   For simulated lip movement I will use 4-3x3 matrix blocks which will
react to the serial output pushed to the speech module.    

2 Components 
~~~~~~~~~~~~~

2.1 Speakers 
=============

2.2 Arduino Uno - 2 boards (1 for speech , 1 for everything else) 
==================================================================

2.3 Battery pack/ ON-Off 
=========================

2.4 4-3*3 led matrices for simulated lip movement 
==================================================

2.5 shift registers 
====================

3 TASKS 
~~~~~~~~

3.1 DONE 1st iteration Makefile 
================================
   DEADLINE: <2014-02-04 Tue>

3.2 TODO Chris Kuethe tutorial seperate dir 
============================================

3.3 TODO Serial input from keyboard 
====================================
   DEADLINE: <2014-02-12 Wed>

3.4 TODO Serial 2 way commune 
==============================

3.5 TODO Text to speach testing for arduino 
============================================
   DEADLINE: <2014-02-12 Wed>

3.6 TODO ?   Using a SD card for storage 
=========================================
   DEADLINE: <2014-02-14 Fri>

3.7 TODO Using libraries in C 
==============================
   DEADLINE: <2014-02-07 Fri>

3.8 TODO Accessing storage in C 
================================
   DEADLINE: <2014-02-09 Sun>

3.9 TODO Parts list for "project" 
==================================
   DEADLINE: <2014-02-09 Sun>

4 SERIAL 
~~~~~~~~~

4.1 Output 
===========
-this does get the output from the serial port 
cat /dev/ttyACM0 

4.2 Input 
==========
haven't tested yet
echo run > /dev/ttyACM0 # supposed to get serial output from Computer

5 Compile-Pipeline 
~~~~~~~~~~~~~~~~~~~

5.1 Command Line build-&-upload 
================================
Compile C-code:
  $ avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o <c_file_name>.o <c_file_name>.c

Object.o to Executable: 
 $  avr-gcc -mmcu=atmega328p <c_file_name>.o -o <executable_name>

ObjectCopy to Hex:
  $  avr-objcopy -O ihex -R .eeprom <executable_name> <executable_name>.hex


Upload to Board:
  *You might have to run this as super user!! BE AWARE * 
  $ sudo avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:<executable_name>.hex

5.1.1 Example: 
---------------
 $ avr-gcc -mmcu=atmega328p blink_led.o -o blink_led
 $ avr-objcopy -O ihex -R .eeprom blink_led blink_led.hex
 $ sudo avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:blink_led.hex

5.2 Makefile build-&-upload 
============================

6 Bugs-&-Fixes 
~~~~~~~~~~~~~~~

6.1 1 
======
COMMAND: avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:blink_led.hex

ERROR:
avrdude: ser_open(): can't open device "/dev/ttyACM0": Permission denied
ioctl("TIOCMGET"): Invalid argument

ANSWER:
Run avrdude as sudo!

6.2 2 
======

6.2.1 ERROR: warning: #warning "F_CPU not defined for <util/delay.h>" [-Wcpp] 
------------------------------------------------------------------------------
Error comes from an undefined preprocessor statement in the FILE.c
Chris's tutorial puts this define in the compilation pipeline itself
But when using a makefile that didn't fix the problem. I don't know why.

As a note, <util/delay.h> will use some defualt to define this but on the
chromebook this resulted in a super fast blinking LED that burned out after
30 seconds or so.  !!!!BE WARNED
[http://www.avrfreaks.net/index.php?name=PNphpBB2&file=printview&t=126600&start=0]

6.2.2 FIX:  put the define in FILE.c itself. 
---------------------------------------------
This allows the compilation with the makefile without a warning 
"#define F_CPU 16000000UL" 

7 c-arduino basics 
~~~~~~~~~~~~~~~~~~~

chaos@chaos ~/Dev/Arduino/267-proj $  avr-gcc -mmcu=atmega328p blink_led.o -o blink_led
chaos@chaos ~/Dev/Arduino/267-proj $  avr-objcopy -O ihex -R .eeprom blink_led blink_led.hex
chaos@chaos ~/Dev/Arduino/267-proj $ avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:blink_led.hex

avrdude: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.00s

avrdude: Device signature = 0x1e950f
avrdude: NOTE: FLASH memory has been specified, an erase cycle will be performed
         To disable this feature, specify the -D option.
avrdude: erasing chip
avrdude: reading input file "blink_led.hex"
avrdude: input file blink_led.hex auto detected as Intel Hex
avrdude: writing flash (202 bytes):

Writing | ################################################## | 100% 0.04s

avrdude: 202 bytes of flash written

avrdude: safemode: Fuses OK

avrdude done.  Thank you.

8 Tool-Chain 
~~~~~~~~~~~~~

8.1 sudo apt-get install make avr-libc avrdude binutils-avr gcc-avr gdb-avr 
============================================================================

9 Tutorials 
~~~~~~~~~~~~

9.1 German robot course 
========================
svn checkout [http://ornella.iwr.uni-heidelberg.de/svn/basis]

9.2 Amercan_Tutorials 
======================

9.2.1 Chris Kuethe--[https://www.mainframe.cx/~ckuethe/avr-c-tutorial/] 
------------------------------------------------------------------------
[https://www.mainframe.cx/~ckuethe/avr-c-tutorial/pin\_map.html]


[http://canthack.org/2010/12/programming-the-arduino-in-pure-c/]
[http://balau82.wordpress.com/2011/03/29/programming-arduino-uno-in-pure-c/]
AVR C [https://www.mainframe.cx/~ckuethe/avr-c-tutorial/]  
[http://www.mjoldfield.com/atelier/2009/02/arduino-cli.html]
[http://johanneshoff.com/arduino-command-line.htm]
[http://kartikmohta.com/tech/avr/tutorial/]


[https://www.mainframe.cx/~ckuethe/avr-c-tutorial/pin\_map.html]: https://www.mainframe.cx/~ckuethe/avr-c-tutorial/pin_map.html

10 Arduino-Uno 
~~~~~~~~~~~~~~~
[http://arduino.cc/en/uploads/Main/arduino-uno-schematic.pd]

11 AVR-info 
~~~~~~~~~~~~
[http://download-mirror.savannah.gnu.org/releases//avrdude/avrdude-doc-5.10.pdf]

12 Links 
~~~~~~~~~
[http://code.google.com/p/tinkerit/wiki/Cantarino]
[http://forum.arduino.cc/index.php?topic=49654.0]
[http://forum.arduino.cc/index.php/topic,136758.0.html]
[http://letsmakerobots.com/node/33388]


XXXX
Clive Webster (Webbot)
[http://www.societyofrobots.com/member\_tutorials/node/211]

[http://www.speechchips.com/shop/]
[http://www.speechchips.com/shop/item.aspx?itemid=22]
[http://www.speechchips.com/shop/category.aspx?catid=3]

XX?
[http://brittonkerin.com/cduino/]

[http://www.societyofrobots.com/member\_tutorials/node/211]: http://www.societyofrobots.com/member_tutorials/node/211

13 AtmEGA328-datasheet++ 
~~~~~~~~~~~~~~~~~~~~~~~~~

13.1 Pins & Ports 
==================
Pin TOTAL= 28 PDIP style

13.1.1 Ports 
-------------
VCC - digital supply voltage
GND - ground
PortB - (PB7:0) XTAL1/XTAL2/TOSC1/TOSC2
PortC - (PC5:0)
PC6/RESET
PortD - (PD7:0)
AVcc - AVCC is the supply voltage pin for the A/D Converter, PC3:0, and ADC7:6 
AREF is the analog reference pin for the A/D Converter.
ADC7:6(TQFP and QFN/MLF package only) ?? what the fuck does that mean?

13.2 Qtouch capacitive touch sensing 
=====================================
Atmel® offers the QTouch® library for embedding capacitive touch buttons, sliders and wheels functionality into
AVR® microcontrollers. The patented charge-transfer signal acquisition offers robust sensing and includes fully
debounced reporting of touch keys and includes Adjacent Key Suppression® (AKSTM) technology for unambiguous
detection of key events. The easy-to-use QTouch Suite toolchain allows you to explore, develop and debug your
own touch applications.

13.3 Program on the fly SPI 
============================
The device is manufactured using Atmel’s high density non-volatile memory technology. The On-chip ISP Flash
allows the program memory to be reprogrammed In-System through an SPI serial interface, by a conventional non-
volatile memory programmer, or by an On-chip Boot program running on the AVR core. The Boot program can use
any interface to download the application program in the Application Flash memory. Software in the Boot Flash
section will continue to run while the Application Flash section is updated, providing true Read-While-Write opera-
tion. By combining an 8-bit RISC CPU with In-System Self-Programmable Flash on a monolithic chip, the Atmel
ATmega48A/PA/88A/PA/168A/PA/328/P is a powerful microcontroller that provides a highly flexible and cost effec-
tive solution to many embedded control applications.


[http://www.atmel.com/images/doc8453.pdf]
