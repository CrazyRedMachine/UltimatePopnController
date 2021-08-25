#define BOUNCE_WITH_PROMPT_DETECTION
#include <Bounce2.h>
#if defined(ARDUINO_ARCH_SAM)
#include <Keypad.h>
#include <Keyboard.h>
#else
#include <EEPROM.h>
/* PSX DEFINE */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define SPI_PORT PORTB
#define SPI_PINS PINB
#define SPI_DDR  DDRB
#define SPI_PINS  PINB
#define ACK_PIN   5 //PB1
#define ATT_PIN   0 //~SS
#define CMD_PIN   2 //MOSI
#define DATA_PIN  3 //MISO
#define CLK_PIN   1 //SCK
#define DATA_LEN 5
/* /PSX DEFINE */
#endif
#include "POPNHID.h"
/* 1 frame (as declared in POPNHID.cpp) on highspeed USB spec is 125µs */
#define REPORT_DELAY 125
#define MILLIDEBOUNCE 15
POPNHID_ POPNHID;

/* Buttons + Lights declarations */
#if defined(ARDUINO_ARCH_SAM)
byte LightPins[] = {A7, CANTX, A8, A9, CANRX, A10, DAC1, A11, DAC0, 14, 15, 16, 17, 18, A6, A5, A4, A3, 19, 20};
byte ButtonPins[] = {5, 4, 3, 2, 1, 6, 7, 8, 9, 11, 13, 10, 12};
byte DipPins[] = {22, 24, 26, 28};
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
//byte rowPins[ROWS] = {46, 44, 42, 40}; //connect to the row pinouts of the keypad
//byte colPins[COLS] = {48, 50, 52}; //connect to the column pinouts of the keypad

/* For mini keypad
*/
byte rowPins[ROWS] = {50, 40, 42, 46}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {48, 52, 44}; //connect to the column pinouts of the keypad

Keypad kpd = Keypad( makeKeymap(numpad), rowPins, colPins, ROWS, COLS );
#else
/* PSX globals */
volatile uint8_t data_buff[DATA_LEN]={0x41,0x5A,0xBF,0xFF,0xFF};//Reply.
volatile uint8_t command_buff[DATA_LEN]={0x01,0x42,0x00,0x00,0x00};

volatile uint8_t curr_byte=0;
volatile uint8_t next_byte=0;

byte b4;
byte b5;

void convertPopn(uint32_t buttons){

    b4 = 0xFF;
    b5 = 0xFF;

    if (((buttons >> 9) & 1)) {
        b4 &= ~((unsigned char) 0x01);
    } else {
        b4 |= ((unsigned char) 0x01);
    }
    if (((buttons >> 10) & 1)) {
        b4 &= ~((unsigned char) 0x08);
    } else {
        b4 |= ((unsigned char) 0x08);
    }

    if (((buttons >> 0) & 1)) {
        b5 &= ~((unsigned char) 0x10);
    } else {
        b5 |= ((unsigned char) 0x10);
    }
    if (((buttons >> 1) & 1)) {
        b5 &= ~((unsigned char) 0x20);
    } else {
        b5 |= ((unsigned char) 0x20);
    }
    if (((buttons >> 2) & 1)) {
        b5 &= ~((unsigned char) 0x08);
    } else {
        b5 |= ((unsigned char) 0x08);
    }
    if (((buttons >> 3) & 1)) {
        b5 &= ~((unsigned char) 0x40);
    } else {
        b5 |= ((unsigned char) 0x40);
    }
    if (((buttons >> 4) & 1)) {
        b5 &= ~((unsigned char) 0x04);
    } else {
        b5 |= ((unsigned char) 0x04);
    }
    if (((buttons >> 5) & 1)) {
        b5 &= ~((unsigned char) 0x80);
    } else {
        b5 |= ((unsigned char) 0x80);
    }
    if (((buttons >> 6) & 1)) {
        b5 &= ~((unsigned char) 0x02);
    } else {
        b5 |= ((unsigned char) 0x02);
    }
    if (((buttons >> 7) & 1)) {
        b4 &= ~((unsigned char) 0x10);
    } else {
        b4 |= ((unsigned char) 0x10);
    }
    if (((buttons >> 8) & 1)) {
        b5 &= ~((unsigned char) 0x01);
    } else {
        b5 |= ((unsigned char) 0x01);
    }
    // left down right always held with popn controller
    b4 &= 0x1F;

    data_buff[2] = b4;
    data_buff[3] = b5;

}
ISR(SPI_STC_vect) {  
  uint8_t inbyte=SPDR;

  if (inbyte==command_buff[curr_byte]) {    
    SPDR = data_buff[curr_byte];
    curr_byte++;
    if (curr_byte<DATA_LEN) {//ACK low.
      //SPI_PORT &= ~(1<<ACK_PIN);
      PORTD &= ~(1<<5);//set HIGH
    // simulate open drain
    //  SPI_DDR |= (1<<ACK_PIN);//output
    //  SPI_PORT &= ~(1<<ACK_PIN);//set low (simulate open collector LOW)

      _delay_us(5);
    //SPI_PORT |= (1<<ACK_PIN);
     PORTD |= (1<<5);//set HIGH

    // simulate open drain
    //  SPI_DDR &= ~(1<<ACK_PIN);//input
    //  SPI_PORT |= (1<<ACK_PIN);//set high (simulate open collector Hi-Z)

    } else {
      SPDR = 0xFF;
      curr_byte=0;
    }
  } else {
    SPDR = 0xFF;
    curr_byte=0;
  }

   SPI_DDR |= (1<<ATT_PIN);//out
   SPI_PORT |= (1<<ATT_PIN);//set HIGH  
   _delay_us(5);
   SPI_PORT &= ~(1<<ATT_PIN);//set HIGH  
}
/* /PSX globals */
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
  for (int i = 0; i < 4; i++) {
    pinMode(DipPins[i], INPUT_PULLUP);
  }
  kpd.setDebounceTime(30);
  Keyboard.begin();
  
/* activate numlock if you are not using the toprow keys */
/*  delay(2000);
  Keyboard.press(136 + 83);
  delay(500);
  Keyboard.release(136+83);
  */
  POPNHID.setLightMode(2);
#else
  uint8_t lightMode = 2;
  EEPROM.get(0, lightMode);
  if (lightMode > 4)
    lightMode = 2;
  POPNHID.setLightMode(lightMode);
  
  /* PSX setup */
  // use TXLED as ACK
 // DDRD |= (1<<5);//output
  PORTD |= (1<<5);//set HIGH

  SPI_DDR |= (1<<DATA_PIN);//output
  SPI_PORT |= (1<<DATA_PIN);//set HIGH  

  SPI_DDR |= (1<<ATT_PIN);//out
  delay(500);
  SPI_PORT |= (1<<ATT_PIN);//set HIGH  
  delay(5000);
  SPI_PORT |= (1<<ATT_PIN);//set HIGH  

  //SPI setup.
//  PRR &= ~(1<<PRSPI);//Set to 0 to ensure power to SPI module.
  //SPSR|=(1<<SPI2X);//Fosc/32. @16MHz==500KHz.
  SPCR|=(1<<SPR1);//Fosc/64. @16MHz==250KHz.
  SPCR|=(1<<CPHA);//Setup @ leading edge, sample @ falling edge.
  SPCR|=(1<<CPOL);//Leading edge is falling edge, trailing edge is rising edge.
  SPCR &= ~(1<<MSTR);//MSTR bit is zero, SPI is slave.
  SPCR|=(1<<DORD);//Byte is transmitted LSB first, MSB last.
  SPCR|=(1<<SPE);//Enable SPI.
  SPCR|=(1<<SPIE);//Enable Serial Transfer Complete (STC) interrupt.
  SPDR=0xFF;

  sei();//Enable global interrupts.
  /* /PSX setup */
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
#if defined(ARDUINO_ARCH_AVR) 
  /* PSX convert */
  convertPopn(buttonsState);
#endif
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
  if (mode == 2){
      if ((millis()-POPNHID.getLastHidUpdate()) > 3000)
        mode = 0;
      else
        mode = 1;
  }
  switch (mode)
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
      if (mode > 4) mode = 0;
      POPNHID.setLightMode(mode);
#if defined(ARDUINO_ARCH_AVR)
      EEPROM.put(0, mode);
#endif
    }
    else if (!(buttonsState&2)) {
      modeChanged = false;
    }
  }
}

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

/* ARDUINO DUE ONLY FUNCTIONS */

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
