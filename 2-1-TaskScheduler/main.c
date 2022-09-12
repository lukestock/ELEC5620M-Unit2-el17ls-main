/*
 * Simple Task Scheduler Example
 * ------------------------------
 *
 * This program shows how to create a function pointer
 * It also shows how to use an array of pointers to build a simple task scheduler.
 */
//Include ResetWDT() macro: 
#include "HPS_Watchdog/HPS_Watchdog.h"

//Define new data type for function which takes an int parameter and returns nothing.
typedef void (*TaskFunction)(int);

//Some functions
void toggle (int id){
    volatile unsigned int *LEDR_ptr = (unsigned int *) 0xFF200000;
    unsigned int ledVal = *LEDR_ptr; //Read
    ledVal = (ledVal ^ (1 << id));   //Modify. Use XOR to toggle LED "id"
    *LEDR_ptr = ledVal;              //Write
}
void invert (int id){ //id not used, but needed to match function pointer data type
    volatile unsigned int *LEDR_ptr = (unsigned int *) 0xFF200000;
    *LEDR_ptr = ~*LEDR_ptr;  ///Invert all LEDs.
}
void clear(int id){ //id not used, but needed to match function pointer data type
    volatile unsigned int *LEDR_ptr = (unsigned int *) 0xFF200000;
    *LEDR_ptr = 0;  ///Clear all LEDs.
}

//Main Function
int main(void) {
    /* Pointers to peripherals */

    // ARM A9 Private Timer Load
    volatile unsigned int *private_timer_load      = (unsigned int *) 0xFFFEC600;
    // ARM A9 Private Timer Value
    volatile unsigned int *private_timer_value     = (unsigned int *) 0xFFFEC604;
    // ARM A9 Private Timer Control
    volatile unsigned int *private_timer_control   = (unsigned int *) 0xFFFEC608;
    // ARM A9 Private Timer Interrupt
    volatile unsigned int *private_timer_interrupt = (unsigned int *) 0xFFFEC60C;
    /* Local Variables */
    const unsigned int taskCount = 4;
    unsigned int taskID = 0;
    unsigned int taskLastTime[taskCount];
    unsigned int taskInterval[taskCount];
    TaskFunction taskFunctions[taskCount] = {&toggle,&toggle,&toggle,&invert};

    /* Initialisation */

    //Initialise task values
    for (taskID = 0; taskID < taskCount; taskID++) {
        taskLastTime[taskID] = *private_timer_value;      //All tasks start now
        taskInterval[taskID] = 100000000 * (1 << taskID); //Make up some intervals
    }
    clear(0);
    // Configure the ARM Private Timer
    *private_timer_load      = 0xFFFFFFFF; 
    *private_timer_control   = (0 << 8) | (0 << 2) | (1 << 1) | (1 << 0);

    /* Main Run Loop */

    while(1) {
        // Read the current time
        unsigned int currentTimerValue = *private_timer_value;
        // Check if it is time to run each task
        for (taskID = 0; taskID < taskCount; taskID++) {
            if ((taskLastTime[taskID] - currentTimerValue) >= taskInterval[taskID]) {
                // When the task is ready to be run, call function, taskID is param
                taskFunctions[taskID](taskID);
                // To avoid accumulation errors, we make sure to mark the last time
                // the task was run as when we expected to run it. Counter is going
                // down, so subtract the interval from the last time.
                taskLastTime[taskID] = taskLastTime[taskID] - taskInterval[taskID];
            }
        }
        // Make sure we clear the private timer interrupt flag if it is set
        if (*private_timer_interrupt & 0x1) {
            // If the timer interrupt flag is set, clear the flag
            *private_timer_interrupt = 0x1;
        }
        // Finally, reset the watchdog timer.
        HPS_ResetWatchdog();
    }
}
