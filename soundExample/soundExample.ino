/*
 * This example plays every .WAV file it finds on the SD card in a loop
 * Conversations with the original Parry taken from 
 * https://tools.ietf.org/html/rfc439
 */
#include <WaveHC.h>
#include <WaveUtil.h>
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

Adafruit_8x8matrix matrixLeft = Adafruit_8x8matrix();
Adafruit_8x8matrix matrixRight = Adafruit_8x8matrix();

/*
BAKERY.WAV  

BTHR_ME.WAV  

DNT_TCH.WAV  

HELLO.WAV  

OUCH.WAV  

PAY_ATN.WAV  

WADIMEAN.WAV  
*/


char
  wav0[]   = "HELLO.WAV",
  wav1[]   = "OUCH.WAV",
  wav2[]   = "PAY_ATN.WAV",
  wav3[]   = "DNT_TCH.WAV";

/*
const char *wavname[]= { wav0, wav1, wav2 };
*/
SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the volumes root directory
FatReader f;      // A FatReader instance other than root, needed for selecting files for wave object

WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time

uint8_t dirLevel; // indent level for file/dir names    (for prettyprinting)
dir_t dirBuf;     // buffer for directory reads

//#define DEBOUNCE 100   //button debouncer
/*
 * Define macro to put error messages in flash memory
 */
#define error(msg) error_P(PSTR(msg))



//const uint8_t b12 = 12;  //false
// Function definitions (we define them here, but the code is below)
void play(FatReader &dir);
void storeEntryName(char name[], dir_t &dir);
void playcomplete( char *name);
void playfile( char *name);

static const uint8_t PROGMEM
  open_leftEye_bmp[] =
  { B00111100,
    B01000100,
    B10011010,
    B10100101,
    B10100101,
    B10011001,
    B01000010,
    B00111100 },
    open_rightEye_bmp[] =
  { B00111100,
    B01000010,
    B10011001,
    B10100101,
    B10100101,
    B10011001,
    B01000010,
    B00111100 },
    
   closeEye_bmp[] =
  { B00000000,
    B00000000,
    B00111000,
    B01000110,
    B11111111,
    B00000000,
    B00000000,
    B00000000 },
    slit_bmp[] =
    {B00000000,
    B00000000,
    B00000000,
    B00000000,
    B11111111,
    B00000000,
    B00000000,
    B00000000 },
      
 slant_rightEye_bmp[] =
  { B10000000,
    B00100000,
    B00001000,
    B00010100,
    B00100010,
    B01000111,
    B00101110,
    B00011100 };
    
    
    
 uint8_t
  //blinkIndex[] PROGMEM = { 1, 2, 3, 4, 3, 2, 1 }, // Blink bitmap sequence
  blinkCountdown = 100, // Countdown to next blink (in frames)
  gazeCountdown  =  75, // Countdown to next eye movement
  gazeFrames     =  50, // Duration of eye movement (smaller = faster)
  mouthPos       =   0, // Current image number for mouth
  mouthCountdown =  10, // Countdown to next mouth change
  newPos         = 255, // New mouth position for current frame
  *seq,                 // Animation sequence currently being played back
  idx,                  // Current array index within animation sequence
  prevBtn        = 99,  // Button # pressed on last loop() iteration
  btnCount       = 0;   // Number of iterations same button has been held
int8_t
  eyeX = 3, eyeY = 3,   // Current eye position
  newX = 3, newY = 3,   // Next eye position
  dX   = 0, dY   = 0;   // Distance from prior to new position
  

const long  DEBOUNCE = 100;

byte Start_Flag = 1;
byte Win_Flag = 0;
long  last_debounce_time = 0;
uint8_t switch0  = 0;
uint8_t switch1 = 1;
uint8_t switch11 = 11;

uint8_t pin_flag= 0; // pin_flag carries the button value to the audio logic
//////////////////////////////////// SETUP
void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps for debugging
  
  putstring_nl("\nTalk-a-lot-Bot!");  // say we woke up!
  
  //for (uint8_t k= 11; k < 14; k++)  
/*
  Set Button Pins to High for user to tell Button Input is Working
*/
  pinMode(switch0, INPUT);
  digitalWrite(switch0, HIGH);   //SET button to high
  

  pinMode(switch11, INPUT);
  digitalWrite(switch11, HIGH);   //SET button to high
  
   pinMode(switch1, INPUT);
  digitalWrite(switch1, HIGH);   //SET button to high

/*I2C addresses given to each matrix  */
  matrixLeft.begin(0x70);  // pass in the address
  matrixRight.begin(0x71);
  
  
  putstring("Free RAM: ");       // This can help with debugging, running out of RAM is bad
  Serial.println(FreeRam());

  //  if (!card.init(true)) { //play with 4 MHz spi if 8MHz isn't working for you
  if (!card.init()) {         //play with 8 MHz spi (default faster!)  
    error("Card init. failed!");  // Something went wrong, lets print out why
  }
  
  // enable optimize read - some cards may timeout. Disable if you're having problems
  card.partialBlockRead(true);
  
  // Now we will look for a FAT partition!
  uint8_t part;
  for (part = 0; part < 5; part++) {   // we have up to 5 slots to look in
    if (vol.init(card, part)) 
      break;                           // we found one, lets bail
  }
  if (part == 5) {                     // if we ended up not finding one  :(
    error("No valid FAT partition!");  // Something went wrong, lets print out why
  }
  
  // Lets tell the user about what we found
  putstring("Using partition ");
  Serial.print(part, DEC);
  putstring(", type is FAT");
  Serial.println(vol.fatType(), DEC);     // FAT16 or FAT32?
  
  // Try to open the root directory
  if (!root.openRoot(vol)) {
    error("Can't open root dir!");      // Something went wrong,
  }
  
  // Whew! We got past the tough parts.
  putstring_nl("Files found (* = fragmented):");

  // Print out all of the files in all the directories.
  root.ls(LS_R | LS_FLAG_FRAGMENTED);
}


///FIXME:  SPEAKER PLUG MUST BE HELD BEFORE A CONNECTION IS MADE AND SOUND PLAYED. 
///FIXME: BUTTONS CAN BE READ WITH FLAGS BUT NEED SOME WAY OF KNOWING WHEN TO LET GO 
/// SO THAT READING DOESN'T INTERFERE WITH PLAYING THE AUDIO !!!!!!!!!!!!!!!!!!!!!
/// LADY ADA : http://www.ladyada.net/learn/arduino/lesson5.html
//////////////////////////////////// LOOP
void loop() {
  uint8_t swp0 = digitalRead(switch0);
  uint8_t swp11 = digitalRead(switch11);
  uint8_t swp1 = digitalRead(switch1);
  //pin_flag 
  Serial.print("pin_flag = ");
  Serial.println(pin_flag);
  uint8_t i;
          /*EYES BLINKING BEHAVIOR   */
 
/* Start the eyes with a wide eye and then a blink */
 
 /* Beginning WaveFile */   
 if ( Start_Flag ){
      Serial.println( "STARTING");
      playcomplete(wav0);
  }
  Start_Flag = 0; 
  eye_move();




  if (pin_flag==10){   
   playcomplete("BAKERY.WAV");
   Serial.println("Playing sound");
 }
 
 
 if (pin_flag==1){
   
   playcomplete("HELLO.WAV");
   Serial.println("Playing sound");
 }
 
 if (pin_flag==11){
   
   playcomplete("BTHR_ME.WAV");
   Serial.println("Playing sound");
 }
  delay(30);
  
 
 if (swp0 == LOW){
   Serial.println("REading button 0");
   Serial.println(swp0);
   pin_flag = 10;
 } 
 else if (swp11 == LOW){

   Serial.println("REading button 11");
   Serial.println(swp11);
   pin_flag = 11;
 } else if (swp1 == LOW){
     Serial.println("REading button 1");
     Serial.println(swp1);
     pin_flag = 1;
 } else {
   pin_flag = 0;
 }
 Serial.print("pin_flag = ");
   Serial.println(pin_flag);
  eye_move();
 
  eye_move();
  
  //playcomplete("GT4.WAV");
  //root.rewind();
  //play(root);



}



void eye_move(){
  
  //Serial.println("EYE MOVE");
    matrixRight.clear();
  matrixRight.drawBitmap(0, 0, open_rightEye_bmp, 8, 8, LED_ON);
  matrixRight.writeDisplay();
  delay(250);

  matrixLeft.clear();
  matrixLeft.drawBitmap(0, 0, open_leftEye_bmp, 8, 8, LED_ON);
  matrixLeft.writeDisplay();
  delay(250);
  
  matrixRight.clear();
  matrixRight.drawBitmap(0, 0, closeEye_bmp, 8, 8, LED_ON);
  matrixRight.writeDisplay();
  delay(500);
  
  matrixLeft.clear();
  matrixLeft.drawBitmap(0, 0, closeEye_bmp, 8, 8, LED_ON);
  matrixLeft.writeDisplay();
  delay(500);
  //Serial.println("LEAVE EYEMOVE");
}

/////////////////////////////////// HELPERS
/*
 * print error message and halt
 */
void error_P(const char *str) {
  PgmPrint("Error: ");
  SerialPrint_P(str);
  sdErrorCheck();
  while(1);
}
/*
 * print error message and halt if SD I/O error, great for debugging!
 */
void sdErrorCheck(void) {
  if (!card.errorCode()) return;
  PgmPrint("\r\nSD I/O error: ");
  Serial.print(card.errorCode(), HEX);
  PgmPrint(", ");
  Serial.println(card.errorData(), HEX);
  while(1);
}

void storeEntryName(char name[], dir_t &dir){
  uint8_t j = 0;
 for (uint8_t i = 0; i < 11; i++) {
  if (dir.name[i] == ' ')continue;
  if (i == 8) name[j++]= '.';
  name[j++] = dir.name[i];
 }
  name[j]=0;
}

/*
 * play recursively - possible stack overflow if subdirectories too nested
 */
void play(FatReader &dir) {
  FatReader file;
  while (dir.readDir(dirBuf) > 0) {    // Read every file in the directory one at a time
  
    // Skip it if not a subdirectory and not a .WAV file
    if (!DIR_IS_SUBDIR(dirBuf)
         && strncmp_P((char *)&dirBuf.name[8], PSTR("WAV"), 3)) {
      continue;
    }

    Serial.println();            // clear out a new line
    
    for (uint8_t i = 0; i < dirLevel; i++) {
       Serial.write(' ');       // this is for prettyprinting, put spaces in front
    }
    if (!file.open(vol, dirBuf)) {        // open the file in the directory
      error("file.open failed");          // something went wrong
    }
    
    if (file.isDir()) {                   // check if we opened a new directory
      putstring("Subdir: ");
      printEntryName(dirBuf);
      Serial.println();
      dirLevel += 2;                      // add more spaces
      // play files in subdirectory
      play(file);                         // recursive!
      dirLevel -= 2;    
    }

    else {
 
      putstring("CLOSE EYE ");
       matrixRight.clear();
       matrixRight.drawBitmap(0, 0, closeEye_bmp, 8, 8, LED_ON);
       matrixRight.writeDisplay();
       delay(500);
  
        putstring("CLOSE EYE");
       matrixLeft.clear();
       matrixLeft.drawBitmap(0, 0, closeEye_bmp, 8, 8, LED_ON);
       matrixLeft.writeDisplay();
       delay(500);
  
      // Aha! we found a file that isnt a directory
      putstring("Playing ");
      printEntryName(dirBuf);       // print it out
      Serial.println();
      putstring(" Name collected from file");
      char name[11];
      storeEntryName(name,dirBuf);
      Serial.println(name);
      
      if (!wave.create(file)) {            // Figure out, is it a WAV proper?
        putstring(" Not a valid WAV");     // ok skip it
      } else {
        Serial.println();                  // Hooray it IS a WAV proper!
        wave.play();                       // make some noise!
        
        uint8_t n = 0;
        while (wave.isplaying) {// playing occurs in interrupts, so we print dots in realtime
          putstring(".");
          if (!(++n % 32))Serial.println();
          delay(100);
        }       
        sdErrorCheck();                    // everything OK?
        // if (wave.errors)Serial.println(wave.errors);     // wave decoding errors
      }
    }
  }
}

// Plays a full file from beginning to end with no pause.
void playcomplete( char *name) {
// call our helper to find and play this name
playfile(name);
while (wave.isplaying) {
// do nothing while its playing
}
// now its done playing
}


void playfile( char *name) {
// see if the wave object is currently doing something
if (wave.isplaying) {// already playing something, so stop it!
wave.stop(); // stop it
}
// look in the root directory and open the file
if (!f.open(root, name)) {
putstring("Couldn't open file "); Serial.print(name); return;
}
// OK read the file and turn it into a wave object
if (!wave.create(f)) {
putstring_nl("Not a valid WAV"); return;
}
// ok time to play! start playback
wave.play();
}
