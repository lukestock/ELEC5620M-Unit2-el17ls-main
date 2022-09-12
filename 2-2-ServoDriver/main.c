/*
 * DE1-SoC Servo Example
 *
 *  Created on: 14 Dec 2017
 *      Author: Thomas Carpenter
 */

#include "DE1SoC_Servo/DE1SoC_Servo.h"
#include "HPS_Watchdog/HPS_Watchdog.h"

#include <stdlib.h>

//
// Fatal Status Check
// ==================
//
// This can be used to terminate the program (i.e. crash + reboot)
// if the "status" value returned from a driver doesn't match the
// "successStatus" value.
//
// You can add a break point on the exit line during debugging to
// see why the function crashed.
//
void exitOnFail(signed int status, signed int successStatus){
    if (status != successStatus) {
        exit((int)status); //Add breakpoint here to catch failure
    }
}

//
// Main Function
// =============
//
int main (void) {

	volatile unsigned int *KEY_ptr = (unsigned int *)0xFF200050;  // KEY buttons base

    /* Pointers to Private Timer */
    // ARM A9 Private Timer Load
    volatile unsigned int  *private_timer_load      = (unsigned int *) 0xFFFEC600;
    // ARM A9 Private Timer Value
    volatile unsigned int  *private_timer_value     = (unsigned int *) 0xFFFEC604;
    // ARM A9 Private Timer Control
    volatile unsigned int  *private_timer_control   = (unsigned int *) 0xFFFEC608;
    // ARM A9 Private Timer Interrupt
    volatile unsigned int  *private_timer_interrupt = (unsigned int *) 0xFFFEC60C;
    /* Local Variables */ 
    unsigned int lastServoUpdate = *private_timer_value;
    const unsigned int servoUpdatePeriod = 10000; 
    signed int servoStep = 0;
    /* Initialisation */
    //Initialise Servo Driver
    exitOnFail(
            Servo_initialise(0xFF2000C0),   //Initialise Servo Controller
            SERVO_SUCCESS);                 //Exit if not successful
    exitOnFail(
            Servo_enable(0, true),          //Enable Servo 0.
            SERVO_SUCCESS);                 //Exit if not successful
    exitOnFail(
            Servo_pulseWidthRange(0, true), //Double pulse width.
            SERVO_SUCCESS);                 //Exit if not successful
    ResetWDT(); //Reset watchdog
    // Configure the ARM Private Timer
    // Set the "Load" value of the timer to max value:
    *private_timer_load      = 0xFFFFFFFF; 
    // Set the "Prescaler" value to 0, Enable the timer (E = 1), Set Automatic reload
    // on overflow (A = 1), and disable ISR (I = 0)
    *private_timer_control   = (0 << 8) | (0 << 2) | (1 << 1) | (1 << 0);
    /* Main Run Loop */
    while (1) {
        // Read the current time
        unsigned int currentTime = *private_timer_value;
        // Check if it is time to update the servo
        if ((lastServoUpdate - currentTime) >= servoUpdatePeriod) {
            // If the time elapsed is enough
        	if (*KEY_ptr & 0x1) {
            signed char posn = 2*abs(servoStep - 127) - 128; //Makes triangle wave
            while (Servo_busy(0) == SERVO_BUSY); //Wait while Servo 0 busy.
            exitOnFail(
                    Servo_pulseWidth(0,posn),  //Update Servo 0 position
                    SERVO_SUCCESS);            //Exit if not successful
            servoStep = (servoStep + 1) % 254; //Move to next step (254 steps)
            // The current time is now the time the LED was last toggled.
            lastServoUpdate = currentTime;
        	}

        	if (*KEY_ptr & 0x2) {
            signed char posn = 2*abs(servoStep - 127); //Makes triangle wave
            while (Servo_busy(0) == SERVO_BUSY); //Wait while Servo 0 busy.
            exitOnFail(
                    Servo_pulseWidth(0,posn),  //Update Servo 0 position
                    SERVO_SUCCESS);            //Exit if not successful
            servoStep = (servoStep + 1) % 254; //Move to next step (254 steps)
            // The current time is now the time the LED was last toggled.
            lastServoUpdate = currentTime;
        	}

        	if (*KEY_ptr & 0x4) {
            signed char posn = 2*abs(servoStep + 127) + 128; //Makes triangle wave
            while (Servo_busy(0) == SERVO_BUSY); //Wait while Servo 0 busy.
            exitOnFail(
                    Servo_pulseWidth(0,posn),  //Update Servo 0 position
                    SERVO_SUCCESS);            //Exit if not successful
            servoStep = (servoStep + 1) % 254; //Move to next step (254 steps)
            // The current time is now the time the LED was last toggled.
            lastServoUpdate = currentTime;
        	}

        	if (*KEY_ptr & 0x8) {
            signed char posn = 2*abs(servoStep + 127) + 128; //Makes triangle wave
            while (Servo_busy(0) == SERVO_BUSY); //Wait while Servo 0 busy.
            exitOnFail(
                    Servo_pulseWidth(0,posn),  //Update Servo 0 position
                    SERVO_SUCCESS);            //Exit if not successful
            servoStep = (servoStep + 2) % 254; //Move to next step (254 steps)
            // The current time is now the time the LED was last toggled.
            lastServoUpdate = currentTime;
        	}
        }
        // Next, make sure we clear the private timer interrupt flag if it is set
        if (*private_timer_interrupt & 0x1) {
            // If the timer interrupt flag is set, clear the flag
            *private_timer_interrupt = 0x1;
        }
        // Finally, reset the watchdog timer.
        HPS_ResetWatchdog(); 
    }
}
