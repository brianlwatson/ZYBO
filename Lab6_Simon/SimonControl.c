//SimonControl.c
#include "SimonControl.h"
#include "supportFiles/display.h"

#define INIT_ZERO 0
#define CONTROL_FOUR 4
#define CONTROL_ONE 1
#define TEXT_SIZE_TWO 2
#define TEXT_SIZE_FOUR 4
#define NEW_LEVEL_SIZE 25
#define SCORE_MESSAGE_SIZE 20
#define NEW_LEVEL_AD_TIMER_EXPIRED 10
#define FINAL_DELAY_EXPIRED 2
#define DISPLAY_SCORE_TIMER_EXPIRED 40
#define SIMON_AD_TIMER_EXPIRED 1
#define WAIT_FOR_PLAYER_TIMER_EXPIRED 30
#define SEQUENCE_SIZE 50
#define LAST_DELAY_EXPIRED 10
#define WAITING_AD_TIMER 20
#define CURSOR_WIDTH display_width()/3.5
#define CURSOR_HEIGHT display_height()/3
#define DISPLAY_WIDTH_EIGHTS display_width()/8
#define DISPLAY_HEIGHT_HALVES display_height()/2
#define DISPLAY_WIDTH_FIFTHS display_width()/5
#define DISPLAY_HEIGHT_THIRDS display_width()/3
#define NEXT_LINE_WIDTH display_width()/7
#define NEXT_LINE_HEIGHT display_height()/3 * 1.5
#define YAY_TIMER_EXPIRED 5
#define LC_EXPIRED 5

#define SIMON "Simon"
#define PROMPT_TOUCH "touch to start game"
#define CONGRATULATIONS "Congratulations!"
#define YAY "Yay!"

uint8_t simonControl_sequence[SEQUENCE_SIZE];
uint16_t sequence_length = SEQUENCE_SIZE;
int16_t wait_for_player_timer = 0;
int16_t simon_ad_timer = 0;
int16_t display_score_timer = 0;
int16_t final_delay = 0;
int16_t new_level_ad_timer = 0;
int16_t level = 1;
int16_t squares_per_level = 4;
uint16_t length = 1;
uint16_t last_delay = 0;
uint16_t waiting_ad_timer = 0;
uint16_t sc_yay_count = 0;
uint16_t pread = 0;
uint16_t lc = 0;
uint16_t max_length = 0;

bool screen_erase = true;
bool screen_display = false;
bool show_score = false;
char newLevel_message[NEW_LEVEL_SIZE];
char score_message[SCORE_MESSAGE_SIZE];


enum simonControl_States {sc_init, sc_waiting, sc_flash_pause, sc_debounce, sc_ad_timer, sc_yay_state, sc_flash, sc_wait_touch, sc_verify, sc_new_level, sc_new_level_ad_timer, sc_display_score,sc_score_running, sc_game_over, sc_last_delay, sc_last_score} sc_state;

//*********************************************************************************************
//@start_message(bool erase)
//@param: bool erase
//Prints a starting message to the touch screen
//If erase is false, the start message will be printed in white
//If erase is true, the start message will be printed over in black
//*********************************************************************************************
void start_message(bool erase)
{
    if(erase == false)
    {
        display_setTextColor(DISPLAY_WHITE);
    }
    
    else
    {
        display_setTextColor(DISPLAY_BLACK);
    }
    
    display_setCursor(CURSOR_WIDTH, CURSOR_HEIGHT);
    display_setTextColor(DISPLAY_WHITE);
    display_setTextSize(TEXT_SIZE_FOUR);
    display_println(SIMON);
    display_setCursor(NEXT_LINE_WIDTH, NEXT_LINE_HEIGHT);
    display_setTextSize(TEXT_SIZE_TWO);
    display_println(PROMPT_TOUCH);
}

//*********************************************************************************************
//@new_level_message(bool erase)
//@param: bool erase
//Prints a starting message to the touch screen
//If erase is false, the new level message will be printed in white
//If erase is true, the new level message will be printed over in black
//*********************************************************************************************
void new_level_message(bool erase)
{
    if(erase == false)
    {
        display_setTextColor(DISPLAY_WHITE);
    }
    
    else
    {
        display_setTextColor(DISPLAY_BLACK);
    }
    
    display_setCursor(DISPLAY_WIDTH_FIFTHS, DISPLAY_HEIGHT_THIRDS );
    display_setTextSize(TEXT_SIZE_TWO);
    display_println(CONGRATULATIONS);
    display_setCursor(NEXT_LINE_WIDTH, NEXT_LINE_HEIGHT);
    display_setTextSize(TEXT_SIZE_TWO);
    sprintf(newLevel_message, "touch to start level %2u", level);
    display_println(newLevel_message);
}

//*********************************************************************************************
//@display_yay(bool erase)
//@param: bool erase
//Prints a starting message to the touch screen
//If erase is false, Yay! will be printed in white
//If erase is true, Yay! will be printed over in black
//*********************************************************************************************
void display_yay(bool erase)
{
    if(erase)
    {
        display_setTextColor(DISPLAY_BLACK);
    }
    else
    {
        display_setTextColor(DISPLAY_WHITE);
    }
    
    display_setCursor(DISPLAY_WIDTH_FIFTHS, DISPLAY_HEIGHT_THIRDS);
    display_setTextSize(TEXT_SIZE_FOUR);
    display_println(YAY);
}

//*********************************************************************************************
//@show_score_message(bool erase)
//@param: bool erase
//Prints a starting message to the touch screen
//If erase is false, the end level score message will be printed in white
//If erase is true, the end level message will be printed over in black
//*********************************************************************************************
void show_score_message(bool erase)
{
    if(erase)
    {
        display_setTextColor(DISPLAY_BLACK);
    }
    else
    {
        display_setTextColor(DISPLAY_WHITE);
    }
    display_setCursor(DISPLAY_WIDTH_EIGHTS, DISPLAY_HEIGHT_HALVES);
    display_setTextSize(TEXT_SIZE_TWO);
    sprintf(score_message, "Longest sequence: %2u", length);
    display_println(score_message);
}

//*********************************************************************************************
//@show_lc(bool erase)
//@param: bool erase
//Prints a starting message to the touch screen
//If erase is false, the longest count message will be printed in white
//If erase is true, the longest count message will be printed over in black
//*********************************************************************************************
void show_lc(bool erase)
{
    if(erase)
    {
        display_setTextColor(DISPLAY_BLACK);
    }
    else
    {
        display_setTextColor(DISPLAY_WHITE);
    }
    display_setCursor(DISPLAY_WIDTH_EIGHTS, DISPLAY_HEIGHT_HALVES);
    display_setTextSize(TEXT_SIZE_TWO);
    sprintf(score_message, "Longest sequence: %2u", max_length);
    display_println(score_message);
}


//*********************************************************************************************
//@screateRandomArray()
// Creates a random number using the rand()
// The array is then set to be the global sequence array
//*********************************************************************************************
void createRandomArray()
{
    for(int i = INIT_ZERO; i < SEQUENCE_SIZE; i++)
    {
        int16_t random = rand() % CONTROL_FOUR;
        simonControl_sequence[i] = random;
    }
    globals_setSequence(simonControl_sequence, sequence_length);
}


//*********************************************************************************************
//@adjustMaxLength(uint16_t num)
//@param: uint16_t num
//Maximizes the max_length integer variable to be printed if the game ends prematurely
//*********************************************************************************************
void adjustMaxLength(uint16_t num)
{
	if(num > max_length)
	{
		max_length = num;
	}
}


//*********************************************************************************************
//@SimonControl_tick()
//The Control State Machine
//*********************************************************************************************
void SimonControl_tick()
{
    switch(sc_state)
    {
        //sc_init
        //Initializes the amount of correct presses to zero
        //Disables the state machines so they do not incorrectly run
        //Displays a start screen
        case sc_init:
        	max_length = INIT_ZERO;
            
            createRandomArray();
            globals_setSequenceIterationLength(length);
            verifySequence_disable();
            flashSequence_disable();
            
            start_message();
            sc_state = sc_waiting;
            break;

            
        //sc_waiting
        //Transitions to the ad_timer debouncer when the screen is touched
        case sc_waiting:
            if(display_isTouched())
            {
                display_clearOldTouchData();
                sc_state = sc_ad_timer;
            }
            break;


        //sc_ad_timer
        //Adds a debounce value for screen presses. Once the value is achieved, it transitions the flash state
        //Else, it returns to the sc_waiting state
        case sc_ad_timer:
            simon_ad_timer++;
            wait_for_player_timer = INIT_ZERO;
            if(display_isTouched() && simon_ad_timer == SIMON_AD_TIMER_EXPIRED)
            {
            	verifySequence_disable();
                start_message(screen_erase);
                flashSequence_enable();
                sc_state = sc_flash;
            }

            else if(!display_isTouched() && simon_ad_timer == SIMON_AD_TIMER_EXPIRED )
            {
                sc_state = sc_waiting;
            }
            break;


        //sc_flash
        //Disables the flashSequence state machine and enables the verifySequence state machine
        case sc_flash:
            simon_ad_timer = INIT_ZERO;
            if(flashSequence_completed())
            {
                flashSequence_disable();
                verifySequence_enable();
                sc_state = sc_verify;
            }
            break;


        //sc_verify
        //Once the verifySequence is complete, a transition will occur based on the conditions of verifyComplete
        //If the user does not press fast enough or presses incorrectly, game over occurs
        //If the user completes a level, the yay state occurs
        //If the user completes a sequence and the level is not over, the next flash sequence occurs
        case sc_verify:
            if(verifySequence_isComplete() && (verifySequence_isTimeOutError() || verifySequence_isUserInputError()) )
            {
                verifySequence_disable();
                
                sc_state = sc_game_over;
            }

            else if(verifySequence_isComplete() && globals_getSequenceIterationLength() == squares_per_level)
            {
                level++;
                verifySequence_disable();
                display_yay(screen_display);
                squares_per_level++;
                
                sc_state = sc_yay_state;
            }

            else if(verifySequence_isComplete() && globals_getSequenceIterationLength() != squares_per_level)
            {
                length++;
            	verifySequence_disable();
                flashSequence_enable();
                adjustMaxLength(length);
                globals_setSequenceIterationLength(length);
                
                sc_state = sc_flash;
            }
            break;

            
        //sc_yay_state
        //Congratulates the user on completing a level
        //The yay message is displayed up until sc_yay_count equals YAY_TIMER_EXPIRED
        //The state machine transitions to a new level
        case sc_yay_state:
        	sc_yay_count++;
        	if(sc_yay_count == YAY_TIMER_EXPIRED)
        	{
        		display_yay(screen_erase);
                
        		sc_state = sc_new_level;
        	}
        	break;

            
        //sc_new_level
        //Offers the user a chance to play a new, longer level
        case sc_new_level:
        	sc_yay_count = INIT_ZERO;
            new_level_message(display_screen);
            
            sc_state = sc_wait_touch;
            break;


        //sc_wait_touch
        //If the user accepts a new level before the time expires, the game will continue
        //else, the final score state is transitioned to
        case sc_wait_touch:
            wait_for_player_timer++;
            
            if(wait_for_player_timer == WAIT_FOR_PLAYER_TIMER_EXPIRED)
            {
            	new_level_message(screen_erase);
                
                sc_state = sc_display_score;
            }

            if(display_isTouched())
            {
                createRandomArray();
                
                sc_state = sc_new_level_ad_timer;
            }
            
            break;


        //sc_display_score
        //Reinitializes various timer values for a new game
        //Shows the score
        case sc_display_score:
            wait_for_player_timer = INIT_ZERO;
            display_score_timer = INIT_ZERO;
            squares_per_level = CONTROL_FOUR;
            level = CONTROL_ONE;
            
            show_score_message(screen_display);
            
            sc_state = sc_score_running;
            break;

            
        //sc_score_running
        //Displays the score for a given amount of time
        //Returns to the init state
        case sc_score_running:
            display_score_timer++;

            if(display_score_timer == DISPLAY_SCORE_TIMER_EXPIRED)
            {
                length = CONTROL_ONE;
                show_score_message(screen_erase);
                
                sc_state = sc_init;
            }

            break;

            
        //sc_new_level_ad_timer
        //Debounces the touch for the user to go to a new level
        //Once touched, the game will begin in the flash state
        case sc_new_level_ad_timer:
            new_level_ad_timer++;
            if(display_isTouched())
            {
                new_level_message(true);
                length = CONTROL_ONE;
                globals_setSequenceIterationLength(length);
                flashSequence_enable();
                new_level_ad_timer=INIT_ZERO;
                
                sc_state = sc_flash;
            }

            else if(!display_isTouched() && new_level_ad_timer == NEW_LEVEL_AD_TIMER_EXPIRED)
            {
                sc_state = sc_wait_touch;
                new_level_ad_timer=INIT_ZERO;
            }

            break;

            
        //sc_game_over
        //Resets values
        //Displays the corresponding game over screen for a time FINAL_DELAY_EXPIRED
        case sc_game_over:
            final_delay++;

            length = CONTROL_ONE;
            if(final_delay == FINAL_DELAY_EXPIRED || verifySequence_isUserInputError())
            {
                final_delay = INIT_ZERO;
                verifySequence_printInfoMessage(user_wrong_sequence_e);
                
                sc_state = sc_last_delay;
            }

            if (verifySequence_isTimeOutError())
            {
                final_delay = INIT_ZERO;
                verifySequence_printInfoMessage(user_time_out_e);

                sc_state = sc_last_delay;
            }
            break;

            
        //sc_last_delay
        //Holds the game over display on the screen for a time LAST_DELAY_EXPIRED
        //Shows the score for the longest count during the game
        case sc_last_delay:
        	last_delay++;
        	if(last_delay == LAST_DELAY_EXPIRED)
        	{
                last_delay = INIT_ZERO;
        		verifySequence_eraseInfoMessage(user_wrong_sequence_e);
        		verifySequence_eraseInfoMessage(user_time_out_e);
                
        		show_lc(display_screen);
                
        		sc_state = sc_last_score;
        	}
        	break;

            
        //sc_last_score
        //Shows up the longest count during the game for time LC_EXPIRED
        case sc_last_score:
        	lc++;
            
        	if(lc == LC_EXPIRED)
        	{
                lc = INIT_ZERO;
        		show_lc(screen_erase);
                
        		sc_state = sc_init;
        	}
        	break;

            
        default:
            sc_state = sc_init;
            break;

    }


}
