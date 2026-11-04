#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>
#include <stdint.h>

// Tasks 1-4
#define MODE_NO_DEBOUNCE 1 // every interrupt counts
#define MODE_TIME_DEBOUNCE 2 // ignore interrupts closer than 50 ms
#define MODE_STATE_DEBOUNCE 3 // accept only if the button is still pressed
#define MODE_POLLING 4 // no interrupts, state machine every 5 ms

// Extra homework
#define MODE_SHORT_PRESS 5 // released before 1 s
#define MODE_LONG_PRESS 6 // held for 1 s
#define MODE_ON_RELEASE 7 // action after release

// Current mode, change to test different modes
#define MODE MODE_STATE_DEBOUNCE

void onButtonInterrupt(void);

bool buttonPressed(uint32_t now);

#endif /* BUTTON_H */
