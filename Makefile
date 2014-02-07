MCU=atmega328p
CC=avr-gcc
OBJCOPY=avr-objcopy
FILE=lesson4
#CPU_FLAG=-DF_CPU=16000000UL
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
CFLAGS=-g -Os -mmcu=$(MCU) -Wall 

#****************
# $ make all -----> Target : all : compiles file.c to final uploadable file.hex
all: $(FILE).hex

#****************
# $ make help ----> Target : help : gives some hints to compiling
help:
	@echo "Usage: make all (must have an appropriate FILE.c for this to build)"
	@echo "Warning: you will most likely have to upload using SUPERUSER sudo command"
	@echo "Usage: make clean (removes all *.hex, *.o , *.out files)"
	@echo "See README.org for complete project help"
#********************************************************


#******************************
# Build instructions for make
$(FILE).hex : $(FILE).out
	$(OBJCOPY) -O ihex -R .eeprom $(FILE).out $(FILE).hex

$(FILE).out : $(FILE).o
	$(CC) $(CFLAGS) -o $(FILE).out $(FILE).o

$(FILE).o : $(FILE).c 
	$(CC) $(CFLAGS) -c $(FILE).c # $(CPU_FLAGS)
# ************************************************


#************************
# $ make clean ----------> Target : clean : gets rid of all *.o , *.out, *.hex files  
clean:
	-rm *.o *.out *.hex

#**************************
