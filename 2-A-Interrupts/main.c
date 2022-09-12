/*
 * main.c
 *
 * This program is a simple piece of code to test interrupts.
 */

#include "HPS_IRQ/HPS_IRQ.h"
#include "HPS_Watchdog/HPS_Watchdog.h"


#define HPS_GPIO_PORT  0
#define HPS_GPIO_DDR   1
#define HPS_GPIO_PIN  20

//HPS Timer Interrupt Toggles LED
void timerISR(HPSIRQSource interruptID, bool isInit, void* initParams) {
    if (!isInit) {
        volatile unsigned int * HPS_timer_ptr = (unsigned int *) 0xFFC08000;
        volatile unsigned int * HPS_gpio_ptr = (unsigned int *) 0xFF709000;
        //Toggle the LED
        unsigned int gpio_rmw;
        gpio_rmw = HPS_gpio_ptr[HPS_GPIO_PORT];
        gpio_rmw = gpio_rmw ^ (1 << 24);
        HPS_gpio_ptr[HPS_GPIO_PORT] = gpio_rmw;
        //Clear the Timer Interrupt Flag
        //By reading timer end of interrupt register
        gpio_rmw = HPS_timer_ptr[3];
    }
    //Reset watchdog.
    HPS_ResetWatchdog();
}

//Key Released Interrupt Displays Last Button Released
void pushbuttonISR(HPSIRQSource interruptID, bool isInit, void* initParams) {
    if (!isInit) {
        volatile unsigned int * KEY_ptr     = (unsigned int *) 0xFF200050;
        volatile unsigned int * HEX0to3_ptr = (unsigned int *) 0xFF200020;
        unsigned int press;
        unsigned int i;
        unsigned int hexVal;
        unsigned char* hexValPtr = (unsigned char*)&hexVal;
        //Read the Push-button interrupt register
        press = KEY_ptr[3];
        //Then clear the interrupt flag by writing the value back
        KEY_ptr[3] = press;
        //Prepare to display 1 or 0 on 7-seg corresponding to whichever keys are pressed.
        for ( i = 0; i < 4; i++) {
            hexValPtr[i] = (press & (1 << i)) ? 0x3F : 0x06;
        }
        // Display on 7-seg LEDs
        HEX0to3_ptr[0] = hexVal;
    }
    //Reset watchdog.
    HPS_ResetWatchdog();
}



int main(void) {
    // IO Pointers
    volatile unsigned int * HPS_timer_ptr = (unsigned int *) 0xFFC08000;
    volatile unsigned int * HPS_gpio_ptr  = (unsigned int *) 0xFF709000;
    volatile unsigned int * KEY_ptr       = (unsigned int *) 0xFF200050;
    // Set GPIO LED to output, and low
    unsigned int gpio_rmw;
    //Set HPS LED low
    gpio_rmw = HPS_gpio_ptr[HPS_GPIO_PORT];
    gpio_rmw = gpio_rmw & ~(1 << 24);
    HPS_gpio_ptr[HPS_GPIO_PORT] = gpio_rmw;
    //Set HPS LED to output
    gpio_rmw = HPS_gpio_ptr[HPS_GPIO_DDR];
    gpio_rmw = gpio_rmw | (1 << 24);
    HPS_gpio_ptr[HPS_GPIO_DDR] = gpio_rmw;
    HPS_ResetWatchdog();

    //Initialise IRQs
    HPS_IRQ_initialise(NULL);
    HPS_ResetWatchdog();

    // Configure HPS Timer to interrupt every second
    // Timer base address
    HPS_timer_ptr[2] = 0; // write to control register to stop timer
    // Set the timer period
    HPS_timer_ptr[0] = 100000000; // period = 1/(100 MHz) x (100 x 10^6) = 1 sec
    // Write to control register to start timer, with interrupts
    HPS_timer_ptr[2] = 0x03; // mode = 1, enable = 1
    // Register interrupt handler for timer
    HPS_IRQ_registerHandler(IRQ_TIMER_L4SP_0, timerISR);
    HPS_ResetWatchdog();

    // Configure Push Buttons to interrupt on press
    KEY_ptr[2] = 0xF;     // Enable interrupts for all four KEYs
    // Register interrupt handler for keys
    HPS_IRQ_registerHandler(IRQ_LSC_KEYS, pushbuttonISR);
    HPS_ResetWatchdog();


    //Do nothing in the run loop but reset the watchdog.
    //Notice how the light blinks anyway.
    while(1) {
        //With interrupts enabled, we can also save power by
        //putting the processor to sleep, until an interrupt
        //occurs. 
        //You don't have to do this if you have other things
        //to do in the main run loop.
        __asm("WFI");
        //Reset watchdog.
        HPS_ResetWatchdog();
    }
}



