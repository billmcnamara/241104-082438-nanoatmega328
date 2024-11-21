#define USE_FASTLED

#if defined(USE_FASTLED)
  #include <FastLED.h>
#else
  #include <WS2812FX.h>
#endif

#if defined(USE_FASTLED)
 #define SET_PIXEL(index, color) leds[index] = color
 #define SHOW_LEDS() FastLED.show()
#else
 #define SET_PIXEL(index, color) ws2812fx.setPixelColor(index, color)
 #define SHOW_LEDS() ws2812fx.show()
#endif

#include <math.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// PINS LED
#if defined(ESP32)
  #define LED_PIN 25
#elif defined(ARDUINO_AVR_NANO)
  #define LED_PIN 13
#elif defined(ARDUINO_BLUEPILL_F103C8)
  #define LED_PIN PB12
#else
  #define LED_PIN 13
#endif
// PINS TEMP
#if defined(ESP32)
  #define ONE_WIRE_BUS 4
#elif defined(ARDUINO_AVR_NANO)
  #define ONE_WIRE_BUS 4
#elif defined(ARDUINO_BLUEPILL_F103C8)
  #define ONE_WIRE_BUS PB4
#else
  #define ONE_WIRE_BUS 4
#endif

#define LED_COUNT 64

#define MATRIX_WIDTH  8
#define MATRIX_HEIGHT 8
#define INTERVAL 12000
#define TIME_FOR_TEMP 3000
#define speed 1000
#define BRIGHTNESS 5
#define FRAMES_PER_SECOND  120
#define COLOR_ORDER GRB
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

// FASTLED FUNCTIONS
#if defined(USE_FASTLED)
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
CRGB leds[LED_COUNT];
CRGB color = CRGB(255, 0, 0);   // Red
CRGB color2 = CRGB(255, 0, 0);  // Red
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, LED_COUNT, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(LED_COUNT) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, LED_COUNT, 10);
  int pos = random16(LED_COUNT);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, LED_COUNT, 20);
  int pos = beatsin16( 13, 0, LED_COUNT-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < LED_COUNT; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, LED_COUNT, 20);
  uint8_t dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, LED_COUNT-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

 // List of patterns to cycle through.  Each is defined as a separate function below.
 typedef void (*SimplePatternList[])();
 SimplePatternList modes = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };
 const uint8_t numModes = 5;
 void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( modes);
    Serial.print("nextPattern is: ");
    Serial.println(gCurrentPatternNumber);
}

void displayNumber(uint8_t number, CRGB color, CRGB color2, bool negative);

#else
  WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
 // Array of mode values to cycle through
 const uint8_t modes[] = {39, 12, 39, 12, 39, 12, 10, 9, 10, 9, 55, 10, 55, 55, 55, 55, 55, 55, 11, 32, 12, 32, 55, 12, 24, 24, 24, 24, 55, 24, 55, 55, 9, 10, 23, 10};
 const uint8_t numModes = sizeof(modes) / sizeof(modes[0]);

 // Array of color values to cycle through
 const uint32_t colors[] = {0xFF0000, 0xFF7700, 0xFFFF00, 0x00FF00, 0xFFFFFF, 0xFF1493};  
 // Red, Orange, Yellow, Green, White, Pink
 const uint8_t numColors = sizeof(colors) / sizeof(colors[0]);
 void displayNumber(uint8_t number, uint32_t color, uint32_t color2, bool negative);

#endif


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

bool negative=false;
bool problem=false;

const byte digitFont[11][6] = {
  {0b111, 0b101, 0b101, 0b101, 0b101, 0b111}, // 0
  {0b010, 0b110, 0b010, 0b010, 0b010, 0b111}, // 1
  {0b111, 0b001, 0b111, 0b100, 0b100, 0b111}, // 2
  {0b111, 0b001, 0b111, 0b001, 0b001, 0b111}, // 3
  {0b101, 0b101, 0b111, 0b001, 0b001, 0b001}, // 4
  {0b111, 0b100, 0b111, 0b001, 0b001, 0b111}, // 5
  {0b111, 0b100, 0b111, 0b101, 0b101, 0b111}, // 6
  {0b111, 0b001, 0b001, 0b001, 0b001, 0b001}, // 7
  {0b111, 0b101, 0b111, 0b101, 0b101, 0b111}, // 8
  {0b111, 0b101, 0b101, 0b111, 0b001, 0b111}, // 9
  {0b000, 0b000, 0b000, 0b111, 0b000, 0b000}  // -
};

const byte smileyFont[8] = {
  0b00111100, // Row 1
  0b01000010, // Row 2
  0b10100101, // Row 3 (eyes)
  0b10000001, // Row 4
  0b10100101, // Row 5 (mouth corners)
  0b10011001, // Row 6 (mouth middle)
  0b01000010, // Row 7
  0b00111100  // Row 8
};


  
unsigned long lastChange = 0;  // To track the last time the mode was changed
uint8_t modeIndex = 0;  // Index for the current mode in the sequence
uint8_t colorIndex = 0;        // Index for the current color in the sequence

void setup() {
  Serial.begin(9600);
  sensors.begin();
  #if defined(USE_FASTLED)
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,LED_PIN,COLOR_ORDER>(leds, LED_COUNT).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  #else
  ws2812fx.init();
  ws2812fx.setBrightness(BRIGHTNESS);
  ws2812fx.setSpeed(speed);
  ws2812fx.setMode(modes[modeIndex]);  // Start with the first mode in the array
  ws2812fx.setColor(colors[colorIndex]);  // Start with the first color
  ws2812fx.start();
  #endif
}

void loop() {
  // Call the current pattern function once, updating the 'leds' array
  // modes[gCurrentPatternNumber]();
  #if defined(USE_FASTLED)
  // send the 'leds' array out to the actual LED strip
    modes[gCurrentPatternNumber]();
    FastLED.show();
    FastLED.delay(1000/FRAMES_PER_SECOND); 
  // do some periodic updates
    EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
    EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
  #else
    ws2812fx.service();
  #endif
  unsigned long now = millis();
  if (now - lastChange >= INTERVAL) {
    #if defined(USE_FASTLED)
    gHue++;
    nextPattern();
    #else
    modeIndex = (modeIndex + 1) % numModes;  // Cycle index within the array size
    colorIndex = (colorIndex + 1) % numColors;   // Cycle through colors
    ws2812fx.setMode(modes[modeIndex]);  // Set the new mode from the array
    ws2812fx.setColor(colors[colorIndex]);       // Set the new color
    #endif
    lastChange = now;  // Update the last change time

    sensors.requestTemperatures();
    float temperature_prefered = 99.99;
    float temperature_X = sensors.getTempCByIndex(0);
    if (isnan(temperature_X)) {
      Serial.println("no temp X!");
      temperature_X = 99;
    } else {
      Serial.print("Temperature_X: ");
      Serial.println(temperature_X);
    }

    if (temperature_X >= 99) {
      Serial.println("Temperature_X >= 99");
      temperature_prefered = 99 ;
    } 
    if (temperature_X <= -99) {
      Serial.println("Temperature_X is < -99, default -99");
      temperature_prefered = -99 ;
    }
    if (temperature_X < 99 && temperature_X > -99 ) {
        Serial.println("Temperature_X is in range -99 -> 99 ");
        temperature_prefered = temperature_X;
    }

    // Digit color mapping based on temperature range
    
    #if defined(USE_FASTLED)
    CRGB color;
    CRGB color2;
    #else
    uint32_t color;
    uint32_t color2;
    #endif

    // temperature_prefered = -60 ;  // uncomment for testing
    // temperature_prefered = -10 ;  // uncomment for testing
    // temperature_prefered = -9 ;  // uncomment for testing
    // temperature_prefered = 9 ;  // uncomment for testing
    // temperature_prefered = 15 ;  // uncomment for testing
    // temperature_prefered = 25 ;  // uncomment for testing
    // negative char for -1 to -9
    negative = false;
    problem = false;
    
    Serial.print("Temperature is: ");
    Serial.println(temperature_prefered);
    if (temperature_prefered < 0) {
        negative = true;
        if (temperature_prefered == -99) {
          problem = true;
          Serial.println("Problem getting Temperature_X.");
        #if defined(USE_FASTLED)
         color = CRGB(255, 0, 0);   // Red
         color2 = CRGB(255, 0, 0);  // Red
        #else
          color = ws2812fx.Color(255, 0 , 0);   // Red
          color2 = ws2812fx.Color(255 ,0 , 0);  // Red
        #endif
        } else {
          problem = false;
        #if defined(USE_FASTLED)
         color = CRGB(255, 255, 255);  // White
         color2 = CRGB(90, 90, 244);   // Whiteish
        #else
          color = ws2812fx.Color(255, 255, 255);  // White
          color2 = ws2812fx.Color(90, 90, 244);   // Whiteish
        #endif
        }
    } else if (temperature_prefered >= 0 && temperature_prefered < 5) {
        negative = false;
        problem = false;
        #if defined(USE_FASTLED)
        color = CRGB(90, 90, 90);     // Black
        color2 = CRGB(255, 255, 255); // Whiteish
        #else
        color = ws2812fx.Color(90, 90, 90);     // Black
        color2 = ws2812fx.Color(255, 255, 255); // Whiteish
        #endif
    } else if (temperature_prefered >= 5 && temperature_prefered < 10) {
        negative = false;
        problem = false;
        #if defined(USE_FASTLED)
        color = CRGB(255, 255, 255);  // White
        color2 = CRGB(90, 90, 244);   // Whiteish
        #else
        color = ws2812fx.Color(255, 255, 255);  // White
        color2 = ws2812fx.Color(90, 90, 244);   // Whiteish
        #endif
    } else if (temperature_prefered >= 10 && temperature_prefered < 13) {
        negative = false;
        problem = false;
        #if defined(USE_FASTLED)
        color = CRGB(0, 0, 255);      // Blue
        color2 = CRGB(119, 119, 255); // Blueish
        #else
        color = ws2812fx.Color(0, 0, 255);      // Blue
        color2 = ws2812fx.Color(119, 119, 255); // Blueish
        #endif
    } else if (temperature_prefered >= 13 && temperature_prefered < 17) {
        negative = false;
        problem = false;
        #if defined(USE_FASTLED)
        color = CRGB(255, 255, 0);    // Yellow
        color2 = CRGB(255, 180, 40);  // Yellowish
        #else
        color = ws2812fx.Color(255, 255, 0);    // Yellow
        color2 = ws2812fx.Color(255, 180, 40);  // Yellowish
        #endif
    } else if (temperature_prefered >= 17 && temperature_prefered < 21) {
        negative = false;
        #if defined(USE_FASTLED)
        color = CRGB(255, 119, 0);  // Orange
        color2 = CRGB(255, 40, 0);  // Orangeish
        #else
        color = ws2812fx.Color(255, 119, 0);  // Orange
        color2 = ws2812fx.Color(255, 40, 0);  // Orangeish
        #endif
    } else if (temperature_prefered >= 21 && temperature_prefered < 99) {
        negative = false;
        problem = false;
        #if defined(USE_FASTLED)
        color = CRGB(255,0 , 0);    // Red
        color2 = CRGB(255 ,60 , 0); // Redish
        #else
        color = ws2812fx.Color(255,0 , 0);    // Red
        color2 = ws2812fx.Color(255 ,60 , 0); // Redish
        #endif
    } else if (temperature_prefered == 99) {
        negative = false;
        problem = false;
        #if defined(USE_FASTLED)
        color = CRGB(255, 0 , 0);   // Red
        color2 = CRGB(255 ,0 , 0);  // Red
        #else
        color = ws2812fx.Color(255, 0 , 0);   // Red
        color2 = ws2812fx.Color(255 ,0 , 0);  // Red
        #endif
    }

    int temperature = (int)abs(floor(temperature_prefered));
    if ( temperature <= 60 || temperature == 99){
      // Display temperature on the matrix only if abs < 60
      displayNumber(temperature, color, color2, negative);
      SHOW_LEDS();
      // show for N secs
      delay(TIME_FOR_TEMP);
    } else {
      SHOW_LEDS();
    }
  }

}

void displaySmiley(byte digit, CRGB color, int xOffset, int yOffset = 1) {
  if (digit != 99) return;  // Only display if 'digit' is 99 for smiley face
  
  for (int y = 0; y < 8; y++) {               // Iterate through all 8 rows
    byte row = smileyFont[y];
    for (int x = 0; x < 8; x++) {             // Iterate through all 8 columns
      int pixelX = x + xOffset;               // Calculate x position with offset
      int pixelY = y + yOffset;               // Calculate y position with offset

      // Ensure within matrix bounds (0-7)
      if (pixelX < 8 && pixelY < 8) {
        int pixelIndex = pixelX + (pixelY * 8); // Convert (x, y) to 1D index

        // Check if the current bit in the row byte is set for the smiley pattern
        if (row & (0b10000000 >> x)) {
      SET_PIXEL(pixelIndex, color);
        } else {
      SET_PIXEL(pixelIndex, 0);
        }
      }
    }
  }
}
void displaySmiley(byte digit, uint32_t color, int xOffset, int yOffset = 1) {
  if (digit != 99) return;  // Only display if 'digit' is 99 for smiley face
  
  for (int y = 0; y < 8; y++) {               // Iterate through all 8 rows
    byte row = smileyFont[y];
    for (int x = 0; x < 8; x++) {             // Iterate through all 8 columns
      int pixelX = x + xOffset;               // Calculate x position with offset
      int pixelY = y + yOffset;               // Calculate y position with offset

      // Ensure within matrix bounds (0-7)
      if (pixelX < 8 && pixelY < 8) {
        int pixelIndex = pixelX + (pixelY * 8); // Convert (x, y) to 1D index

        // Check if the current bit in the row byte is set for the smiley pattern
        if (row & (0b10000000 >> x)) {
      SET_PIXEL(pixelIndex, color);
        } else {
      SET_PIXEL(pixelIndex, 0);
        }
      }
    }
  }
}

void displayDigit(byte digit, CRGB color, int xOffset, int yOffset = 2) {
  if (digit > 10) return;  // Only 0–9 are defined in digitFont and -
  
  for (int y = 0; y < 6; y++) {               // Only 5 rows are used for each digit
    byte row = digitFont[digit][y];
    for (int x = 0; x < 3; x++) {             // Each digit is 3 columns wide
      int pixelX = x + xOffset;               // Calculate x position with offset
      int pixelY = y + yOffset;               // Calculate y position with offset

      // Ensure within matrix bounds (0-7)
      if (pixelX < 8 && pixelY < 8) {
        int pixelIndex = pixelX + (pixelY * 8); // Convert (x, y) to 1D index

        // Check if the current bit in the row byte is set for the digit pattern
        if (row & (0b100 >> x)) {
      SET_PIXEL(pixelIndex, color);
        } else {
      SET_PIXEL(pixelIndex, 0);
        }
      }
    }
  }
}
void displayDigit(byte digit, uint32_t color, int xOffset, int yOffset = 2) {
  if (digit > 10) return;  // Only 0–9 are defined in digitFont and -
  
  for (int y = 0; y < 6; y++) {               // Only 5 rows are used for each digit
    byte row = digitFont[digit][y];
    for (int x = 0; x < 3; x++) {             // Each digit is 3 columns wide
      int pixelX = x + xOffset;               // Calculate x position with offset
      int pixelY = y + yOffset;               // Calculate y position with offset

      // Ensure within matrix bounds (0-7)
      if (pixelX < 8 && pixelY < 8) {
        int pixelIndex = pixelX + (pixelY * 8); // Convert (x, y) to 1D index

        // Check if the current bit in the row byte is set for the digit pattern
        if (row & (0b100 >> x)) {
      SET_PIXEL(pixelIndex, color);
        } else {
      SET_PIXEL(pixelIndex, 0);
        }
      }
    }
  }
}


void clearMatrix() {
    for (int i = 0; i < LED_COUNT; i++) {
        SET_PIXEL(i, 0);
    }
    SHOW_LEDS();
}


void displayNumber(uint8_t number, uint32_t color, uint32_t color2, bool negative) {

  clearMatrix();  // Clear the matrix before displaying the new number
  
  int units = number % 10;  // Units digit
  int tens;

  if ( problem ) {
    displaySmiley(99, color, 0, 0);  // Changed yOffset to 2 for downward movement
  } else {
    units = number % 10;  // Units digit
    if ( ! negative ) {
      tens = number / 10;   // Tens digit
      // Display tens digit on the left side (columns 0–2), with yOffset of 2 (down by 2 rows)
      displayDigit(tens, color, 1, 1);  // Changed yOffset to 2 for downward movement
    } else {
      //we only do up to - 60
      if (number >= 10 && number <= 99) {
          tens = number / 10;   // Tens digit
      } else {
          tens = 10;   // - char
      }
      displayDigit(tens, color, 0, 1);  // Changed yOffset to 2 for downward movement
    }
  // Display units digit on the right side (columns 4–6), with yOffset of 2 (down by 2 rows)
  displayDigit(units, color2, 4, 1);  // Changed yOffset to 2 for downward movement
  }
}


void displayNumber(uint8_t number, CRGB color, CRGB color2, bool negative) {

  clearMatrix();  // Clear the matrix before displaying the new number
  
  int units = number % 10;  // Units digit
  int tens;

  if ( problem ) {
    displaySmiley(99, color, 0, 0);  // Changed yOffset to 2 for downward movement
  } else {
    units = number % 10;  // Units digit
    if ( ! negative ) {
      tens = number / 10;   // Tens digit
      // Display tens digit on the left side (columns 0–2), with yOffset of 2 (down by 2 rows)
      displayDigit(tens, color, 1, 1);  // Changed yOffset to 2 for downward movement
    } else {
      //we only do up to - 60
      if (number >= 10 && number <= 99) {
          tens = number / 10;   // Tens digit
      } else {
          tens = 10;   // - char
      }
      displayDigit(tens, color, 0, 1);  // Changed yOffset to 2 for downward movement
    }
  // Display units digit on the right side (columns 4–6), with yOffset of 2 (down by 2 rows)
  displayDigit(units, color2, 4, 1);  // Changed yOffset to 2 for downward movement
  }
}