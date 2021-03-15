#define BOUNCE_WITH_PROMPT_DETECTION
#include <Bounce2.h>
#include <EEPROM.h>
#include <FastLED.h>
#include "POPNHID.h"
/* 1 frame (as declared in POPNHID.cpp) on fullspeed USB spec is 1ms */
#define REPORT_DELAY 1000
#define MILLIDEBOUNCE 15
POPNHID_ POPNHID;

/* Buttons + Lights declarations */
#define BUT_LED_PIN     A0
#define BUT_NUM_LEDS    9
#define LEFT_LED_PIN     8
#define LEFT_NUM_LEDS    9
#define RIGHT_LED_PIN     9
#define RIGHT_NUM_LEDS    9
CRGB but_leds[BUT_NUM_LEDS];
CRGB left_leds[LEFT_NUM_LEDS];
CRGB right_leds[RIGHT_NUM_LEDS];
CRGB but_led_colors[BUT_NUM_LEDS] = {CRGB::White, CRGB::Green, CRGB::Red, CRGB::Green, CRGB::White, CRGB::Yellow, CRGB::Blue, CRGB::Blue, CRGB::Yellow};
#define PILLAR_BRIGHTNESS 64
#define BUTTON_BRIGHTNESS 220
uint8_t NeonPins[] = {A1, A2, A3, A4, A5};
uint8_t ButtonPins[] = {0, 1, 2, 3, 4, 5, 6, 7, 11, 12, 13};
uint8_t sysPin = 12;
//this array contains the light offset given buttons 0 to 8 as ref
uint8_t lightOrder[] = {0, 8, 1, 7, 2, 6, 3, 5, 4};

const byte ButtonCount = sizeof(ButtonPins) / sizeof(ButtonPins[0]);
const byte NeonCount = sizeof(NeonPins) / sizeof(NeonPins[0]);

Bounce buttons[ButtonCount];

/* SETUP */
void setup() {
  // setup I/O for pins
  for (int i = 0; i < ButtonCount; i++) {
    buttons[i] = Bounce();
    buttons[i].attach(ButtonPins[i], INPUT_PULLUP);
    buttons[i].interval(MILLIDEBOUNCE);
  }

  for (int i = 0; i < NeonCount; i++) {
    pinMode(NeonPins[i], OUTPUT);
  }

  FastLED.addLeds<WS2812, BUT_LED_PIN, GRB>(but_leds, BUT_NUM_LEDS);
  FastLED.addLeds<WS2812, LEFT_LED_PIN, GRB>(left_leds, LEFT_NUM_LEDS);
  FastLED.addLeds<WS2812, RIGHT_LED_PIN, GRB>(right_leds, RIGHT_NUM_LEDS);
  FastLED.setBrightness( BUTTON_BRIGHTNESS );

  uint8_t lightMode;
  EEPROM.get(0, lightMode);
  if (lightMode < 0 || lightMode > 3)
    lightMode = 2;
  POPNHID.setLightMode(lightMode);

  //boot animation
  uint16_t anim[] = {1, 4, 16, 64, 256, 128, 32, 8, 2};
  animate(anim, 9, 100);
  animate(anim, 9, 100);
  uint16_t anim2[] = {1 + 4 + 16 + 64 + 256, 2 + 8 + 32 + 128};
  animate(anim2, 2, 500);
  animate(anim2, 2, 500);
}

/* LOOP */
unsigned long lastReport = 0;
uint32_t prevButtonsState = 0;
bool modeChanged = false;
void loop() {
  /* BUTTONS */
  uint32_t buttonsState = 0;
  for (int i = 0; i < ButtonCount; i++) {
    buttons[i].update();
    int value = buttons[i].read();
    if (value != HIGH) {
      buttonsState |= (uint32_t)1 << i;
    } else {
      buttonsState &= ~((uint32_t)1 << i);
    }
  }

  /* USB DATA */
  if ( ( (micros() - lastReport) >= REPORT_DELAY) )
  {
    POPNHID.sendState(buttonsState);
    lastReport = micros();
    prevButtonsState = buttonsState;

    //check for HID-requested lightmode change
    POPNHID.updateLightMode();
  }

  /* LAMPS */
  uint8_t mode = POPNHID.getLightMode();
  /* mixed mode will behave sometimes like HID, sometimes like reactive */
  if (mode == 2) {
    if ((millis() - POPNHID.getLastHidUpdate()) > 3000)
      mode = 0;
    else
      mode = 1;
  }
  switch (mode)
  {
    /* Reactive mode, locally determined lamp data */
    case 0:
      but_lights(buttonsState & 0x1ff);
      reactive_neon(buttonsState & 0x1ff);
      break;
    /* HID mode, only based on received HID data */
    case 1:
      POPNHID.updateLeds(0, false);
      break;
    /* Combined inverse mode, received HID data and button state are combined then inverted */
    case 4:
      POPNHID.updateLeds(buttonsState & 0x1ff, true);
      break;
    /* Combined mode, received HID data and button state are combined */
    case 3:
      POPNHID.updateLeds(buttonsState & 0x1ff, false);
      break;
    default:
      break;
  }

  /* MANUAL LIGHTMODE UPDATE */
  if ( buttonsState & 1024 ) {
    if ( (buttonsState & 2) && (modeChanged == false)) {
      modeChanged = true;
      uint8_t mode = POPNHID.getLightMode() + 1;
      if (mode > 4) mode = 0;
      POPNHID.setLightMode(mode);
      EEPROM.put(0, mode);
    }
    else if (!(buttonsState & 2)) {
      modeChanged = false;
    }
  }
}

/* Light up button lights according to bitfield */
void but_lights(uint16_t lightDesc) {
  //conversion de lightDesc
  lightDesc = convert_order(lightDesc);

  //allumage
  for (int i = 0; i < BUT_NUM_LEDS; i++) {
    if ((lightDesc >> i) & 1) {
      but_leds[i] = but_led_colors[i];
    } else {
      but_leds[i] = 0;
    }
  }
  FastLED.show();
}

/* Light up pillars and top neons according to bitfield */
void neon_lights(uint16_t lightDesc) {
  //Top-Lamp
  for (int i = 0; i < NeonCount; i++) {
    if ((lightDesc >> i) & 1) {
      digitalWrite(NeonPins[i], HIGH);
    }    else  {
      digitalWrite(NeonPins[i], LOW);
    }
  }

  //LEFT
  CRGB left_color = 0;
  if ((lightDesc >> 5) & 1) left_color.b += PILLAR_BRIGHTNESS;
  if ((lightDesc >> 6) & 1) left_color.r += PILLAR_BRIGHTNESS;
  for (int i = 1; i < 8; i++) {
    left_leds[i] = left_color;
  }
  //RIGHT
  CRGB right_color = 0;
  if ((lightDesc >> 7) & 1) right_color.b += PILLAR_BRIGHTNESS;
  if ((lightDesc >> 8) & 1) right_color.r += PILLAR_BRIGHTNESS;
  for (int i = 1; i < 8; i++) {
    right_leds[i] = right_color;
  }


  FastLED.show();
}

/* Display animation on the cab according to a bitfield array */
void animate(uint16_t* tab, uint8_t n, int mswait) {
  for (int i = 0; i < n; i++) {
    but_lights(tab[i]);
    neon_lights(tab[i]);
    delay(mswait);
  }
}

/* Manage pillars and top neons in reactive mode */
uint16_t neon_anim[] = {16, 24, 28, 30, 31, 30, 28, 24};
int neon_anim_index = 0;
uint16_t pillar_state[] = {0, 0x140, 0xA0, 0x1E0};
int pillar_state_index = 0;
bool pillar_lit = false;
uint16_t prevState = 0;
unsigned long lastBlink = 0;
unsigned long lastNeonUpdate = 0;
unsigned long lastButtonAction = 0;
unsigned long actionRate = 0;
unsigned long neonRate = 200;
void reactive_neon(uint16_t buttonsState) {
  uint16_t neons = 0;
  unsigned long currTime = millis();

  /*
     SIDE PILLARS
     When pressing any button the side pillars will blink for half a second
     The color is randomly chosen with blue being predominant, red rare and purple super rare
  */
  if ( buttonsState != prevState )
  {
    if ( buttonsState != 0 ) {
      long randNumber = random(21);
      if (randNumber == 0)
      {
        pillar_state_index = 3;
      }
      else if (randNumber < 3)
      {
        pillar_state_index = 1;
      } else {
        pillar_state_index = 2;
      }
      pillar_lit = true;
      if (buttonsState != 0) lastButtonAction = currTime;
    }
    prevState = buttonsState;
  } else { /* no state change, continue to blink for 50ms */
    if (currTime - lastBlink > 50) {
      pillar_lit = !pillar_lit;
      lastBlink = currTime;
    }
    if (currTime - lastButtonAction > 500) {
      pillar_state_index = 0;
    }
  }

  if (pillar_lit)
    neons |= pillar_state[pillar_state_index];

  /*
     Adjusting top neon animation speed (should go faster when you hit buttons quickly)
  */
  actionRate = currTime - lastButtonAction;

  if (15 * actionRate < neonRate)
    neonRate *= 0.99995;
  else if (actionRate > 10 * neonRate) {
    neonRate = neonRate * 1.01;
    if (neonRate < 100) neonRate++;
  }
  if (neonRate > 400)
    neonRate = 400;

  if (neonRate < 40)
    neonRate = 40;

  /*
     Cycling through the top neon animation
  */
  if ((currTime - lastNeonUpdate) > neonRate)
  {
    neon_anim_index++;
    if (neon_anim_index > 7) neon_anim_index = 0;
    lastNeonUpdate = currTime;
  }

  neons |= neon_anim[neon_anim_index];

  /*
     Light the leds
  */
  neon_lights(neons);
}

/* BARTOP ONLY FUNCTIONS */
uint16_t convert_order(uint16_t lightDesc) {
  uint16_t res = 0;
  for (int i = 0; i < BUT_NUM_LEDS; i++) {
    if ((lightDesc >> i) & 1) {
      res |= 1 << (lightOrder[i]);
    }
  }
  return res;
}
