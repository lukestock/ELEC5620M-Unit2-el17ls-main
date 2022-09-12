/*
 * This program provides a non-blocking example of code for the ARM A9 Private Timer
 */
//Include ResetWDT() macro:
#include "HPS_Watchdog/HPS_Watchdog.h"
//Main Function
int main(void) {

	unsigned int i = 0;
	unsigned int lastTimerValue[10] = {0};
	const unsigned int requiredPeriod[10] = {100000000, 200000000, 300000000, 40000000, 500000000, 600000000, 700000000, 800000000, 900000000, 1000000000};
    /* Pointers to peripherals */
    // Red LEDs base address
    volatile unsigned int *LEDR_ptr = (unsigned int *) 0xFF200000;
    // ARM A9 Private Timer Load
    volatile unsigned int *private_timer_load      = (unsigned int *) 0xFFFEC600;
    // ARM A9 Private Timer Value
    volatile unsigned int *private_timer_value     = (unsigned int *) 0xFFFEC604;
    // ARM A9 Private Timer Control
    volatile unsigned int *private_timer_control   = (unsigned int *) 0xFFFEC608;
    // ARM A9 Private Timer Interrupt
    volatile unsigned int *private_timer_interrupt = (unsigned int *) 0xFFFEC60C;
    /* Local Variables */ 
    unsigned int lastBlinkTimerValue = *private_timer_value;
    //const unsigned int blinkPeriod = 100000000;
    /* Initialisation */ 
    // Set initial value of LEDs
    *LEDR_ptr = 0x0;
    // Configure the ARM Private Timer
    // Set the "Load" value of the timer to max value:
    *private_timer_load      = 0xFFFFFFFF; 
    // Set the "Prescaler" value to 0, Enable the timer (E = 1), Set Automatic reload
    // on overflow (A = 1), and disable ISR (I = 0)
    *private_timer_control   = (0 << 8) | (0 << 2) | (1 << 1) | (1 << 0);
    /* Main Run Loop */
    while(1) { 
        // Read the current time
        unsigned int currentTimerValue = *private_timer_value;

        // Pass 10 MSBs of timer value to the 10 red LEDs
        //*LEDR_ptr = currentTimerValue >> 22;

        for (i = 0; i < 10; i++){
			if ((lastTimerValue[i] - currentTimerValue) >= requiredPeriod[i]) {
				 // When the difference between the last time and current time is greater
				 // than the required period. We use subtraction to prevent glitches
				 // when the timer value overflows:
				 // e.g. (0x10 - 0xFFFFFFFF) & 0xFFFFFFFF = 0x11.
				 // If the time elapsed is enough, perform our actions.

				unsigned int ledVal = *LEDR_ptr; //Read
				ledVal = (ledVal ^ (1 << i)); //Modify. Use XOR to toggle LED "id"
				*LEDR_ptr = ledVal; //Write

				 // To avoid accumulation errors, we make sure to mark the last time
				 // the task was run as when we expected to run it. Counter is going
				 // down, so subtract the interval from the last time.
				 lastTimerValue[i] = lastTimerValue[i] - requiredPeriod[i];
				 }
    }

		if (*private_timer_interrupt & 0x1) {
		            // If the timer interrupt flag is set, clear the flag
		    *private_timer_interrupt = 0x1;
		}

		// Old code for blinking leds

        // Check if it is time to blink
        //if ((lastBlinkTimerValue - currentTimerValue) >= blinkPeriod) {
            // When the difference between the last time and current time is greater
            // than the required blink period. We use subtraction to prevent glitches
            // when the timer value overflows:
            //      e.g. (0x10 - 0xFFFFFFFF) & 0xFFFFFFFF = 0x11.
            // If the time elapsed is enough, perform our actions.
            //*LEDR_ptr = ~(*LEDR_ptr); // Invert the LEDs
            // To avoid accumulation errors, we make sure to mark the last time
            // the task was run as when we expected to run it. Counter is going
            // down, so subtract the interval from the last time.
            //lastBlinkTimerValue = lastBlinkTimerValue - blinkPeriod;
        //}
        // --- You can have many other events here by giving each an if statement
        // --- and its own "last#TimerValue" and "#Period" variables, then using the
        // --- same structure as above.
        // Next, make sure we clear the private timer interrupt flag if it is set

        // Finally, reset the watchdog timer.
        HPS_ResetWatchdog();
    }
}
