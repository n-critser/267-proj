/* file: blink_led.c
   -----------------------------
   description: Currently this has loops that do nothing.  It just increases the flash rate of the
                on board LED connected to I/O 13 .  This can be extended to a breadboard which will
                by running a wire from pin13 to on lead of an LED and running another wire to GND
                (ground).  
   -------------------------------------------------------------------------------------------

   build: $ avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o blink_led.o blink_led.c
          $ avr-gcc -mmcu=atmega328p blink_led.o -o blink_led
          $ avr-objcopy -O ihex -R .eeprom blink_led blink_led.hex
          $ sudo avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:blink_led.hex
   upload:
          1: find the port--->
          chaos@chaos ~/Dev/Arduino/267-proj $ lsusb    
          Bus 001 Device 002: ID 8087:0024 Intel Corp. Integrated Rate Matching Hub
          Bus 002 Device 002: ID 8087:0024 Intel Corp. Integrated Rate Matching Hub
          Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
          Bus 002 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
          Bus 001 Device 008: ID 2341:0043 Arduino SA Uno R3 (CDC ACM) !!!!!!!!<------HERE i AM
          Bus 002 Device 004: ID 059f:0321 LaCie, Ltd 
          Bus 002 Device 005: ID 058f:6254 Alcor Micro Corp. USB Hub
          Bus 002 Device 003: ID 058f:a001 Alcor Micro Corp. 
          Bus 002 Device 008: ID 046d:c064 Logitech, Inc. 
          Bus 002 Device 009: ID 03f0:5511 Hewlett-Packard DeskJet F300 series
          Bus 002 Device 006: ID 046d:c31c Logitech, Inc. Keyboard K120 for Business
          Bus 002 Device 007: ID 058f:6254 Alcor Micro Corp. USB Hub

          2: 

 */
/* 3.5 Tip #5 – Constants in program space    */
#define F_CPU 16000000UL 
#include <avr/io.h>
#include <util/delay.h>



int main (void){

        uint8_t leds = 1;
        /* set PORTB for output */
        DDRB = 0xFF;
        while(1) {
                for (leds = 1; leds <= (1 << 5); leds <<= 1){
                        PORTB = leds;
                        _delay_ms(200);
                }

                for (leds = (1 <<4); leds >= 2; leds >>=1) {
                        PORTB = leds;
                        _delay_ms(200);
                }
               
        }
        return 0;
}
                
