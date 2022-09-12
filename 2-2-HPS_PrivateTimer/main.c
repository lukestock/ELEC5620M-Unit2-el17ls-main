/*
 * main.c
 *
 *  Created on: 3 Apr 2022
 *      Author: Luke Stock
 */
#include "HPS_PrivateTimer.h"



int main() {

	    // Configure the ARM Private Timer using functions
	    Timer_initialise(0xFFFEC600);      // Timer base address
	    Timer_setLoadval(0xFFFFFFFF);      // Set to maximum value
	    Timer_setControlvals(0, 0, 1, 1); // (prescaler, I, E, A): Set clock frequency - prescaler, disable ISR - 0,  enable automatic overflow - A, enable timer - E

}


