#include "ps2.h"

#if defined(ARDUINO_ARCH_AVR)
/* USER CUSTOMIZABLE SETTINGS */
#define ACK_PORT  PORTD
#define ACK_DDR   DDRD
#define ACK_PIN   5     // PD5 (TXLED) 

/* RECOMMENDED DO NOT CHANGE */
#define INVERT_CIPO 0  // Set to 1 if CIPO is open-drain via transistor (recommended)
#define INVERT_ACK  0  // Set to 1 if ACK is open-drain via transistor (recommended)

/* END OF USER CUSTOMIZABLE SETTINGS */

// Stores a constructed packet for the PS2.
uint16_t Data = 0;
// List of available PS2 inputs.
PS2_InputList_t *PS2Input = NULL;
// Current PS2 state.
void (*PS2Handler)(uint8_t) = NULL;



void PS2_Acknowledge(void) {
  // Burn a few cycles before acknowledging
  asm volatile(
    "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"
  );
#if INVERT_ACK == 0
  ACK_DDR  |= (1<<ACK_PIN);
#else
  ACK_DDR  &= ~(1<<ACK_PIN);
#endif
  // 40 cycles of delay should give us the same delay as a real PS1 controller
  asm volatile(
    "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"
    "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"
    "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"
    "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"
    "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"
  );
#if INVERT_ACK == 0
  ACK_DDR  &= ~(1<<ACK_PIN);
#else
  ACK_DDR  |= (1<<ACK_PIN);
#endif
}

void PS2_Listen(uint8_t in);
void PS2_Addressed(uint8_t in);
void PS2_HeaderFinished(uint8_t in);
void PS2_LowerSent(uint8_t in);

uint8_t memory_card_timeout = 0;

void PS2_MemoryCardTimeout(uint8_t in) {
  if  (memory_card_timeout) --memory_card_timeout;
  if (!memory_card_timeout)
    PS2Handler = PS2_Listen;
}

void PS2_MemoryCardID2(uint8_t in) {
 // If we receive a 0x01 here, revert back to the listener
  --memory_card_timeout;
  if (in == 0x01) {
    memory_card_timeout = 0;
    PS2_Listen(in);
    return;
  }
  PS2Handler = PS2_MemoryCardTimeout;
}

void PS2_MemoryCardID1(uint8_t in) {
 // If we receive a 0x01 here, revert back to the listener
  --memory_card_timeout;
  if (in == 0x01) {
    memory_card_timeout = 0;
    PS2_Listen(in);
    return;
  }
  PS2Handler = PS2_MemoryCardID2;
}

// Memory card addressed; ignore input
void PS2_MemoryCardAddressed(uint8_t in) {
  memory_card_timeout = 8;
  if (in == 'R')
    memory_card_timeout = 138;
  if (in == 'W')
    memory_card_timeout = 136;

  PS2Handler = PS2_MemoryCardID1;
}

// Idle state.
void PS2_Listen(uint8_t in) {
  if (in == 0x81) {
    DDRB &= ~0x08;
    PS2Handler = PS2_MemoryCardAddressed;
  }
  // Report as a digital controller when addressed
  if (in == 0x01) {
    DDRB |= 0x08;
#if INVERT_CIPO == 1
    SPDR = ~0x41;
#else
    SPDR = 0x41;
#endif
    PS2Handler = PS2_Addressed;
    PS2_Acknowledge();
  }
}

// When polling is requested, begin responding
void PS2_Addressed(uint8_t in) {
  if (in == 0x42) {
#if INVERT_CIPO == 1
    SPDR = ~0x5A;
#else
    SPDR = 0x5A;
#endif
    PS2Handler = PS2_HeaderFinished;
    PS2_Acknowledge();
  }
}

// After end-of-header sent, send the first byte
void PS2_HeaderFinished(uint8_t in) {
  uint8_t *data = (uint8_t *)&Data;

#if INVERT_CIPO == 1
  SPDR = ~(*data);
#else
  SPDR = (*data);
#endif
  PS2Handler = PS2_LowerSent;
  PS2_Acknowledge();
}

// After first byte sent, send the second and go back to listening.
void PS2_LowerSent(uint8_t in) {
  uint8_t *data = (uint8_t *)&Data + 1;

#if INVERT_CIPO == 1
  SPDR = ~(*data);
#else
  SPDR = (*data);
#endif
  PS2Handler = PS2_Listen;
  PS2_Acknowledge();
}

void PS2_Init(void) {
  cli();

#if INVERT_ACK == 0
  ACK_PORT &= ~(1<<ACK_PIN);
#else
  ACK_PORT |= (1<<ACK_PIN);
#endif
  PS2_Acknowledge();
  // Set MISO as an output pin
  DDRB |= 0x08;
  // Setup data on falling edge, sample on rising edge (SPI mode 3)
  SPCR  = (1 << CPOL) | (1 << CPHA)
  // Transmit LSB first
        | (1 << DORD)
  // Enable interrupts for SPI
        | (1 << SPIE)
  // Enable SPI
        | (1 << SPE);

  // Set the first byte up
  
#if INVERT_CIPO == 1
  SPDR = 0x00; // 0xFF;
#else
  SPDR = 0xFF;
#endif
  PS2Handler = PS2_Listen;
  sei();
}

// Update the stored data packet
void PS2_Task(void) {
  if (PINB & 0x01) 
  {
    
#if INVERT_CIPO == 1
    SPDR = 0x00; 
#else
    SPDR = 0xFF;
#endif
  }

  PS2_InputList_t *map = PS2Input;
  uint16_t new_data = 0;

  while(map) {
    if (!map->input || (*map->input & map->mask))
      new_data |= map->buttons;

    map = map->parent;
  }

  Data = ~new_data;
}

void PS2_MapInput(uint16_t *input, uint16_t mask, PS2_INPUT buttons) {
  PS2_InputList_t *child = calloc(1, sizeof(PS2_InputList_t));

  child->input    = input,
  child->mask     = mask,
  child->buttons  = buttons,
  child->parent   = PS2Input;

  PS2Input = child;
}

void PS2_AlwaysInput(PS2_INPUT buttons) {
  PS2_InputList_t *child = calloc(1, sizeof(PS2_InputList_t));

  child->input    = NULL,
  child->mask     = 0,
  child->buttons  = buttons,
  child->parent   = PS2Input;

  PS2Input = child;
}

// When a transfer is complete, determine what to do next
ISR(SPI_STC_vect) {
  uint8_t input = SPDR;
  if (input == 0x01 && (!memory_card_timeout)) PS2Handler = PS2_Listen;
  PS2Handler(SPDR);
}
#endif
