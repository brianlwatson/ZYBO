#ifndef simonFlashSequence_H_
#define simonFlashSequence_H_

// Turns on the state machine. Part of the interlock.
void flashSequence_enable();

// Turns off the state machine. Part of the interlock.
void flashSequence_disable();

// Other state machines can call this to determine if this state machine is finished.
bool flashSequence_completed();

// Standard tick function.
void flashSequence_tick();

// Tests the flashSequence state machine.
void flashSequence_runTest();

#endif
