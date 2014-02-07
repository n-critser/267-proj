MCU=atmega328p
CC=avr-gcc
OBJCOPY=avr-objcopy
FILE=blink_led
# build: $ avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o blink_led.o blink_led.c
# $ avr-gcc -mmcu=atmega328p blink_led.o -o blink_led
# $ sudo avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:blink_led.hex


#-------------------
#avrm8ledtest.hex : avrm8ledtest.out 
#	$(OBJCOPY) -R .eeprom -O ihex avrm8ledtest.out avrm8ledtest.hex 
#avrm8ledtest.out : avrm8ledtest.o 
#	$(CC) $(CFLAGS) -o avrm8ledtest.out -Wl,-Map,avrm8ledtest.map avrm8ledte
#st.o 

# optimize for size:
#CFLAGS=-g -mmcu=$(MCU) -Wall -Wstrict-prototypes -Os -mcall-prologues
CFLAGS=-g  -Os -DF_CPU=16000000UL -mmcu=$(MCU) -Wall 

$(FILE).o : $(FILE).c 
	$(CC) $(CFLAGS) -Os -c $(FILE).c