/* file: hello_board.c
   build: avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -0 ...



 */
/* 3.5 Tip #5 – Constants in program space    */
#include <avr/io.h>
#include <avr/prmspace.h>

uint8_t string[12] PROGMEM =
{"hello world!"};

int main (void){
        UDRO = pgm_read_byte(&string[10]);
}
                
