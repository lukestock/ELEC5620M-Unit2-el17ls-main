/*
 * This program provides a simple example of code for the ARM A9 Private Timer
 */
//Include ResetWDT() macro:
#include "HPS_Watchdog/HPS_Watchdog.h"
//Main Function
int main(void) {
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
    /* Initialisation */ 
    // Set initial value of LEDs
    *LEDR_ptr = 0x1;                      
    // Configure the ARM Private Timer
    // Set the "Load" value of the timer to 100M:
    *private_timer_load      = 0xFFFFFFFF;
    // Set the "Prescaler" value to 0, Enable the timer (E = 1), Set Automatic reload
    // on overflow (A = 1), and disable ISR (I = 0)
    *private_timer_control   = (200 << 8) | (0 << 2) | (1 << 1) | (1 << 0);
    /* Main Run Loop */ 
    while(1) { 
        // Wait until timer overflows (i.e. the I flag gets set high)
        while(!(*private_timer_interrupt & 0x1)) HPS_ResetWatchdog();
        // Clear the timer interrupt flag. We do this by writing 1 to the F bit, 
        // which counter-intuitively informs the hardware to set the flag to set to 0
        *private_timer_interrupt = 0x1;
        // Invert the LEDs
        *LEDR_ptr = ~(*LEDR_ptr);
        // Reset the watchdog timer
        HPS_ResetWatchdog();
    }
}
