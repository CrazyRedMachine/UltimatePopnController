#define BOUNCE_WITH_PROMPT_DETECTION
#include <Bounce2.h>
#if defined(ARDUINO_ARCH_SAM)
#include <Keypad.h>
#include <Keyboard.h>
#endif
#include "POPNHID.h"
/* 125µs delay is 1 frame on highspeed USB spec */
#define REPORT_DELAY 125
#define MILLIDEBOUNCE 15
POPNHID_ POPNHID;

/* Buttons + Lights declarations */
#if defined(ARDUINO_ARCH_SAM)
byte LightPins[] = {36, 38, 40, 42, 44, 46, 48, 50, 52, 37, 39, 41, 43, 45, 47, 49, 51, 53};
byte ButtonPins[] = {5, 6, 7, 8, 9, 10, 11, 12, 13, 4, 3, 2};
#else
uint8_t LightPins[] = {11,12,13,23,22,21,20,19,18};
uint8_t ButtonPins[] = {0,1,2,3,4,5,6,7,8,9,10};
#endif

const byte ButtonCount = sizeof(ButtonPins) / sizeof(ButtonPins[0]);
const byte LightCount = sizeof(LightPins) / sizeof(LightPins[0]);
Bounce buttons[ButtonCount];

#if defined(ARDUINO_ARCH_SAM)
/* Keypad declarations */
const byte ROWS = 4;
const byte COLS = 3;

/*
// To use the keypad as the numpad keys (will require to send numlock for it to work)
char numpad[ROWS][COLS] = {
  {'\347', '\350', '\351'},
  {'\344', '\345', '\346'},
  {'\341', '\342', '\343'},
  {'\352', ',', '\337'}
};
*/

/* This is to use the toprow keys instead */
char numpad[ROWS][COLS] = {
  {'7', '8', '9'},
  {'4', '5', '6'},
  {'1', '2', '3'},
  {'0', ',', '\337'}
};

/* This follows the Pop'n Music cabinet numpad pins order */
byte rowPins[ROWS] = {A3, A2, A1, A0}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {A4, A5, A6}; //connect to the column pinouts of the keypad

/* For mini keypad
byte rowPins[ROWS] = {A5, A0, A1, A3}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {A4, A6, A2}; //connect to the column pinouts of the keypad
*/

Keypad kpd = Keypad( makeKeymap(numpad), rowPins, colPins, ROWS, COLS );
#endif

/* SETUP */
void setup() {
  // setup I/O for pins
  for (int i = 0; i < ButtonCount; i++) {
        buttons[i] = Bounce();
        buttons[i].attach(ButtonPins[i], INPUT_PULLUP);
        buttons[i].interval(MILLIDEBOUNCE);
  }

  for (int i = 0; i < LightCount; i++) {
    pinMode(LightPins[i], OUTPUT);
  }
  
#if defined(ARDUINO_ARCH_SAM)
  kpd.setDebounceTime(30);
  Keyboard.begin();
  
/* activate numlock if you are not using the toprow keys */
/*  delay(2000);
  Keyboard.press(136 + 83);
  delay(500);
  Keyboard.release(136+83);
  */
#endif
  
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
    if (value != HIGH){
      buttonsState |= (uint32_t)1 << i;
    } else {
      buttonsState &= ~((uint32_t)1 << i);
    }
  }

  /* USB DATA */
  if ( ( (micros() - lastReport) >= REPORT_DELAY) )//&& (buttonsState != prevButtonsState) )
  {
    POPNHID.sendState(buttonsState);
    lastReport = micros();
    prevButtonsState = buttonsState; 
    
    //check for HID-requested lightmode change
    POPNHID.updateLightMode();
  }  
  
  /* LAMPS */
  switch (POPNHID.getLightMode())
  {
    /* Reactive mode, locally determined lamp data */
    case 0:
      but_lights(buttonsState & 0x1ff);
#if defined(ARDUINO_ARCH_SAM)     
      reactive_neon(buttonsState & 0x1ff);
#endif
      break;
    /* HID mode, only based on received HID data */
    case 1:
      POPNHID.updateLeds(0, false);
      break;
    /* Mixed inverse mode, received HID data and button state are combined then inverted */
    case 3:
      POPNHID.updateLeds(buttonsState & 0x1ff, true);
      break;
    /* Mixed mode, received HID data and button state are combined */
    default:
      POPNHID.updateLeds(buttonsState & 0x1ff, false);
      break;
  }
  
#if defined(ARDUINO_ARCH_SAM)
  /* KEYPAD */
  if (kpd.getKeys())
  {
    for (int i = 0; i < LIST_MAX; i++) // Scan the whole key list.
    {
      if ( kpd.key[i].stateChanged )   // Only find keys that have changed state.
      {
        switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
          case PRESSED:
            Keyboard.press(kpd.key[i].kchar);
            break;
          case HOLD:

            break;
          case RELEASED:
            Keyboard.release(kpd.key[i].kchar);
            break;
          case IDLE:
            break;
        }

      }
    }
  }
#endif
  
  /* MANUAL LIGHTMODE UPDATE */
  if ( buttonsState & 1024 ) {
    if ( (buttonsState & 2) && (modeChanged == false)) {
      modeChanged = true;
      uint8_t mode = POPNHID.getLightMode()+1;
      if (mode > 3) mode = 0;
      POPNHID.setLightMode(mode);
    }
    else if (!(buttonsState&2)) {
      modeChanged = false;
    }
  }
}


#if defined(ARDUINO_ARCH_SAM)
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
   * SIDE PILLARS
   * When pressing any button the side pillars will blink for half a second
   * The color is randomly chosen with blue being predominant, red rare and purple super rare
   */
  if ( buttonsState != prevState )
  {
    if ( buttonsState != 0 ){
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
   * Adjusting top neon animation speed (should go faster when you hit buttons quickly)
   */
    actionRate = currTime - lastButtonAction;
 
    if (15*actionRate < neonRate)
    neonRate *= 0.99995;
    else if (actionRate > 10*neonRate){
    neonRate = neonRate*1.01;
    if (neonRate < 100) neonRate++;
    }
    if (neonRate > 400)
    neonRate = 400;

    if (neonRate < 40)
    neonRate = 40;
  
  /*
   * Cycling through the top neon animation
   */
  if ((currTime - lastNeonUpdate) > neonRate)
  {
    neon_anim_index++;
    if (neon_anim_index > 7) neon_anim_index = 0;
    lastNeonUpdate = currTime;
  }

  neons |= neon_anim[neon_anim_index];

  /*
   * Light the leds
   */
  neon_lights(neons);
}
#endif

/* Light up button lights according to bitfield */
void but_lights(uint16_t lightDesc) {
  for (int i = 0; i < 9; i++) {
    if ((lightDesc >> i) & 1) {
      digitalWrite(LightPins[i], HIGH);
    }    else  {
      digitalWrite(LightPins[i], LOW);
    }
  }
}

/* Light up pillars and top neons according to bitfield */
void neon_lights(uint16_t lightDesc) {
  for (int i = 0; i < 9; i++) {
    if ((lightDesc >> i) & 1) {
      digitalWrite(LightPins[i + 9], HIGH);
    }    else  {
      digitalWrite(LightPins[i + 9], LOW);
    }
  }
}

/* Display animation on the cab according to a bitfield array */
void animate(uint16_t* tab, uint8_t n, int mswait) {
  for (int i = 0; i < n; i++) {
    but_lights(tab[i]);
#if defined(ARDUINO_ARCH_SAM)
    neon_lights(tab[i]);
#endif
    delay(mswait);
  }
}


