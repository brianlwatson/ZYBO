
#ifndef VERIFYSEQUENCE_H_
#define VERIFYSEQUENCE_H_

enum verifySequence_infoMessage_t
{
  user_time_out_e,            // means that the user waited too long to tap a color.
  user_wrong_sequence_e,      // means that the user tapped the wrong color.
  user_correct_sequence_e,    // means that the user tapped the correct sequence.
  user_quit_e                 // means that the user wants to quite.
};


// State machine will run when enabled.
void verifySequence_enable();

// This is part of the interlock. You disable the state-machine and then enable it again.
void verifySequence_disable();

// Used to detect if there has been a time-out error.
bool verifySequence_isTimeOutError();

// Used to detect if the user tapped the incorrect sequence.
bool verifySequence_isUserInputError();

// Used to detect if the verifySequence state machine has finished verifying.
bool verifySequence_isComplete();

// Standard tick function.
void verifySequence_tick();

// Standard runTest function.
void verifySequence_runTest();

void verifySequence_printInfoMessage(verifySequence_infoMessage_t messageType);

void verifySequence_eraseInfoMessage(verifySequence_infoMessage_t messageType);

#endif /* VERIFYSEQUENCE_H_ */
