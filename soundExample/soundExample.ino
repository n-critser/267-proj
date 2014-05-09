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



PROGMEM const char
  wav0[]   = "pay_atn.wav",
  wav1[]   = "ouch.wav",
  wav2[]   = "hello.wav";


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
void playcomplete(char *name);
void playfile(char *name);

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
int switchPin = 11;
uint8_t pin_flag= 1;
//////////////////////////////////// SETUP
void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps for debugging
  
  putstring_nl("\nTalk-a-lot-Bot!");  // say we woke up!
  pinMode(switchPin, INPUT);
  digitalWrite(switchPin, HIGH);   //SET button to high
//for (uint8_t k= 11; k < 14; k++)  

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
  uint8_t swp = digitalRead(switchPin);
  //pin_flag 
  Serial.print("pin_flag = ");
  Serial.println(pin_flag);
  uint8_t i;
          /*EYES BLINKING BEHAVIOR   */
  if ( Win_Flag  ){
      Serial.println("YOU WIN !!!!!!!!!!!");
  }    
          
  if ( Start_Flag ){
      //Serial.println( "STARTING NEW GAME ");
      //playcomplete("ENTNUM.WAV");
  }
  Start_Flag = 0; 
  eye_move();
  //Serial.print("Whats the btnCount= ");
  //Serial.println(btnCount);
  
 Serial.println("REading button");
 Serial.println(swp);
 if (pin_flag==1){
   
   playcomplete("HELLO.WAV");
   Serial.println("Playing sound");
 }
 /* if ( check_switches () ){
    Serial.println ("PLAYING NUM4.WAV");
    //playcomplete ( "NUM4.WAV");
    
    eye_move();
    
    
  }

 // Scan buttons 6, 7, 8 looking for first button pressed...
 //for(i=0; (i<3) && (digitalRead(i+11) == LOW); i++);
   Serial.println("REading button");
   Serial.println(digitalRead(11));
  if(i < 3) {               // Anything pressed?  Yes!
    if(i == prevBtn) {      // Same as last time we checked?  Good!
      if(++btnCount == 3) { // 3 passes to 'debounce' button input
        
        playcomplete("HELLO.WAV");
        //playfile((char *)pgm_read_word(&wavname[i])); // Start WAV
        // Look up animation sequence # corresponding to this WAV...
        //seq            = (uint8_t *)pgm_read_word(&anim[i]);
        idx            = 0; // Begin at first byte of data
        //newPos         = pgm_read_byte(&seq[idx++]); // Initial mouth pos
        //mouthCountdown = pgm_read_byte(&seq[idx++]); // Hold time for pos
        Serial.println("REading button");
        Serial.println(digitalRead(11));
      }
    } else btnCount = 0; // Different button than before - start count over
    prevBtn = i;
  } else {
    prevBtn = 99;   // No buttons pressed
    btnCount=0;
  }
  */
  
 Serial.println("REading button");
 Serial.println(digitalRead(switchPin));
 if (swp == LOW){
   Serial.print("pin_flag = ");
   Serial.println(pin_flag);
   pin_flag = 1;
 } else {
   pin_flag = 0;
 }
 Serial.print("pin_flag = ");
   Serial.println(pin_flag);
  eye_move();
 
  delay(100);
  //playcomplete("PAY_ATN.WAV");  
  
  //eye_move();
  //matrixLeft.clear();
  //matrixLeft.drawBitmap(0, 0, closeEye_bmp, 8, 8, LED_ON);
  //matrixLeft.writeDisplay();
  //delay(500);
  
 
  eye_move();
  
  //playcomplete("GT4.WAV");
  //root.rewind();
  //play(root);



}

byte check_switches(){
    Serial.println("check_switches");
    //static long db_time= millis();
    //static byte previous[2];
    //static long time[2];
    byte reading;
    byte pressed;
    byte index = 1;
    pressed = 0;
    delay(100);
    Serial.print("pressed:");
    Serial.println(pressed);
    
    Serial.print("last_debounce_time:");
    Serial.println(last_debounce_time);
    
    //for (byte index = 0; index < 2; ++index) {
    //reading = digitalRead(b12 );
    Serial.print("Reading:");
    Serial.println(reading);
    //if (reading == HIGH  && millis() - last_debounce_time > DEBOUNCE){
    // switch pressed
    
    if (reading == HIGH) {   // removed all the debounce from decision for now !
      Serial.println("ENTERING BUTTON PUSH REGION");
      last_debounce_time = millis();
      pressed = index + 1; //value of pressed
    //break;
    }
    //previous[index] = reading;
    //}
    // return switch number (1 - 2)
    
    Serial.print("pressed");
    Serial.println(pressed);
    return (pressed);
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
void playcomplete(char *name) {
// call our helper to find and play this name
playfile(name);
while (wave.isplaying) {
// do nothing while its playing
}
// now its done playing
}


void playfile(char *name) {
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
