#include <FastLED.h>
#include<string.h>
FASTLED_USING_NAMESPACE

String val;
#define DATA_PIN    13
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    60
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

uint8_t gCurrentPatternNumber = 0; 
uint8_t gHue = 0;


void setup() {
    delay(3000);
    FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);
    Serial.begin(9600);
    Serial.println("BLUETOOTH LED LAMP");
    fill_solid( leds, NUM_LEDS, CHSV(0,0,0));
    FastLED.show();
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void rainbow() {
    fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() {
    rainbow();
    addGlitter(80);
}

void addGlitter(fract8 chanceOfGlitter) {
    if (random8() < chanceOfGlitter) {
        leds[ random16(NUM_LEDS) ] += CRGB::White;
    }
}

void confetti() {
    fadeToBlackBy( leds, NUM_LEDS, 10);
    int pos = random16(NUM_LEDS);
    leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon() {
    fadeToBlackBy( leds, NUM_LEDS, 20);
    int pos = beatsin16( 13, 0, NUM_LEDS-1 );
    leds[pos] += CHSV( gHue, 255, 192);
}

void bpm() {
    uint8_t BeatsPerMinute = 62;
    CRGBPalette16 palette = PartyColors_p;
    uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    }
}

void juggle() {
    fadeToBlackBy( leds, NUM_LEDS, 20);
    byte dothue = 0;
    for( int i = 0; i < 8; i++) {
        leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
        dothue += 32;
    }
}

typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

void nextPattern() {
    gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void loop()
{
    while (Serial.available()) {
        val = Serial.readString();
        Serial.println(val);
        if (val == "red") {
            fill_solid( leds, NUM_LEDS, CRGB::Red);
            FastLED.show();
        } else if (val == "green") {
            fill_solid( leds, NUM_LEDS, CRGB::Green);
            FastLED.show();
        } else if (val == "blue") {
            fill_solid( leds, NUM_LEDS, CRGB::Blue);
            FastLED.show();
        } else if (val == "on") {
            fill_solid( leds, NUM_LEDS, CRGB(255,255,255));
            FastLED.show();
        } else if (val == "off") {
            fill_solid( leds, NUM_LEDS, CRGB(0,0,0));
            FastLED.show();
        } else if (val == "pattern") {
            nextPattern();
            gPatterns[gCurrentPatternNumber]();
            FastLED.show();  
            FastLED.delay(1000/FRAMES_PER_SECOND); 
        } else {
            int value = atoi(val.c_str());
            value = constrain(value, 0, 255);
            fill_solid( leds, NUM_LEDS, CHSV(value,255,BRIGHTNESS));
            FastLED.show();  
        }
        EVERY_N_MILLISECONDS( 20 ) { gHue++; }
    } 
    
    if (val == "pattern") {
        gPatterns[gCurrentPatternNumber]();
        FastLED.show();  
        FastLED.delay(1000/FRAMES_PER_SECOND);
    }
    EVERY_N_MILLISECONDS( 20 ) { gHue++; }
}
