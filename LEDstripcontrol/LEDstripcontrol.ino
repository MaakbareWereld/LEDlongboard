#include <FastLED.h>

#define DATA_PIN 6
#define SPEEDINPIN A0
#define NUM_LEDS 30
#define MAXSPEED 8.3 
#define DEBUG false

CRGB leds[NUM_LEDS];

/* code by Rolf Hut, based on code by Mark Kriegsman, 
 *  the animation shows a bar looping around a neopixel LED-strip.
 *  The speed is determined by reading an analog value from SPEEDINPIN
 *  Everything related to the "integer" bar from Marks code has been removed.
 */

// Anti-aliased light bar example
//   v1 by Mark Kriegsman <kriegsman@tr.org>, November 29, 2013
//
// This example shows the basics of using variable pixel brightness
// as a form of anti-aliasing to animate effects on a sub-pixel level, 
// which is useful for effects you want to be particularly "smooth".
//
// This animation shows two bars looping around an LED strip, one moving
// in blocky whole-pixel "integer" steps, and the other one moving 
// by smoothly anti-aliased "fractional" (1/16th pixel) steps.
// The use of "16ths" (vs, say, 10ths) is totally arbitrary, but 16ths are
// a good balance of data size, expressive range, and code size and speed.
//
// Notionally, "I" is the Integer Bar, "F" is the Fractional Bar.


int     F16pos = 0; // position of the "fraction-based bar"
int     F16delta = 1; // how many 16ths of a pixel to move the Fractional Bar
uint8_t Fhue = 20; // color for Fractional Bar

int Width  = 4; // width of each light bar, in whole pixels
int n=0;

int InterframeDelay = 5; //ms
float speedFactor;
float restFactor=0.0;

unsigned long t0;
unsigned long t1;

void setup() {
  delay(3000); // setup guard
  FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(128);
  if (DEBUG) Serial.begin(115200);
  
  speedFactor = MAXSPEED * 30.001 * 16.001 / (1023.001 * 1000000.001);
  if (DEBUG) Serial.println(speedFactor);
  t0 = micros();
  
}
//end setup()



void loop()
{
  unsigned long voltRead = ((unsigned long)analogRead(SPEEDINPIN));
  t1 = micros();
  float dx =  ((float)(voltRead * (t1 - t0)) * speedFactor) + restFactor;
  restFactor = dx - (float)((unsigned long)dx);
  
  if (DEBUG){
    if ((n++%1024)==0) {
      Serial.print(speedFactor);
      Serial.print(" ");
      Serial.print(restFactor);
      Serial.print(" ");
      Serial.println(t1-t0);
    }
  }
  t0 = t1;
  // Update the "Fraction Bar" by 1/16th pixel every time
  F16pos += (unsigned long) dx;
  
  // wrap around at end
  // remember that F16pos contains position in "16ths of a pixel"
  // so the 'end of the strip' is (NUM_LEDS * 16)
  if( F16pos >= (NUM_LEDS * 16)) {
    F16pos -= (NUM_LEDS * 16);
    Fhue = (Fhue+16)%256;
  }
    
   
   // Draw everything:
   // clear the pixel buffer
   memset8( leds, 0, NUM_LEDS * sizeof(CRGB));
   // draw the Fractional Bar, length=4px, hue=180
   
   drawFractionalBar( F16pos, Width, Fhue);
   
   FastLED.show();
   delay(InterframeDelay);
}
//end loop()




// Draw a "Fractional Bar" of light starting at position 'pos16', which is counted in
// sixteenths of a pixel from the start of the strip.  Fractional positions are 
// rendered using 'anti-aliasing' of pixel brightness.
// The bar width is specified in whole pixels.
// Arguably, this is the interesting code.
void drawFractionalBar( int pos16, int width, uint8_t hue)
{
  int i = pos16 / 16; // convert from pos to raw pixel number
  uint8_t frac = pos16 & 0x0F; // extract the 'factional' part of the position 
  
  // brightness of the first pixel in the bar is 1.0 - (fractional part of position)
  // e.g., if the light bar starts drawing at pixel "57.9", then 
  // pixel #57 should only be lit at 10% brightness, because only 1/10th of it
  // is "in" the light bar:
  //
  //                       57.9 . . . . . . . . . . . . . . . . . 61.9
  //                        v                                      v
  //  ---+---56----+---57----+---58----+---59----+---60----+---61----+---62---->
  //     |         |        X|XXXXXXXXX|XXXXXXXXX|XXXXXXXXX|XXXXXXXX |  
  //  ---+---------+---------+---------+---------+---------+---------+--------->
  //                   10%       100%      100%      100%      90%        
  //
  // the fraction we get is in 16ths and needs to be converted to 256ths,
  // so we multiply by 16.  We subtract from 255 because we want a high
  // fraction (e.g. 0.9) to turn into a low brightness (e.g. 0.1)
  uint8_t firstpixelbrightness = 255 - (frac * 16);

  // if the bar is of integer length, the last pixel's brightness is the 
  // reverse of the first pixel's; see illustration above.
  uint8_t lastpixelbrightness  = 255 - firstpixelbrightness;
  
  // For a bar of width "N", the code has to consider "N+1" pixel positions,
  // which is why the "<= width" below instead of "< width".
  
  uint8_t bright;
  for( int n = 0; n <= width; n++) {
    if( n == 0) {
      // first pixel in the bar
      bright = firstpixelbrightness;
    } else if( n == width ) {
      // last pixel in the bar
      bright = lastpixelbrightness;
    } else {
      // middle pixels
      bright = 255;
    }
    leds[i] += CHSV( hue, 255, bright);
    i++; 
    if( i == NUM_LEDS) i = 0; // wrap around
  }
}
