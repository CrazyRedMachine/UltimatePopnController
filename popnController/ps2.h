#if defined(ARDUINO_ARCH_AVR)

#include <stdlib.h>
#include <stdint.h>
#include <avr/interrupt.h>

typedef enum {
  PS2_NC        = 0,
  PS2_SELECT    = (1 << 0),
  PS2_L3        = (1 << 1),
  PS2_R3        = (1 << 2),
  PS2_START     = (1 << 3),
  PS2_UP        = (1 << 4),
  PS2_RIGHT     = (1 << 5),
  PS2_DOWN      = (1 << 6),
  PS2_LEFT      = (1 << 7),
  PS2_L2        = (1 << 8),
  PS2_R2        = (1 << 9),
  PS2_L1        = (1 << 10),
  PS2_R1        = (1 << 11),
  PS2_TRIANGLE  = (1 << 12),
  PS2_CIRCLE    = (1 << 13),
  PS2_CROSS     = (1 << 14),
  PS2_SQUARE    = (1 << 15),
} PS2_INPUT;

typedef struct PS2_InputList_t PS2_InputList_t;

struct PS2_InputList_t {
  uint16_t *input;          // Input source; NULL is always true
  uint16_t  mask;           // Mask to check; (*input & mask)
  PS2_INPUT buttons;        // OR the following if true
  PS2_InputList_t *parent;
};

#ifdef __cplusplus
extern "C"{
#endif

void PS2_Init(void);
void PS2_Task(void);

void PS2_MapInput(uint16_t *input, uint16_t mask, PS2_INPUT buttons);
void PS2_AlwaysInput(PS2_INPUT buttons);

#ifdef __cplusplus
}
#endif

#endif
