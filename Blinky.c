/*------------------------------------------------------------------------------
 * Copyright (c) 2018 Arm Limited (or its affiliates). All
 * rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Blinky.c
 * Purpose: LED Flasher
 *----------------------------------------------------------------------------*/

#include "cmsis_os2.h"                  // ARM::CMSIS:RTOS:Keil RTX5
#include "Board_LED.h"                  // Board Support:LED
#include "Board_Buttons.h"              // Board Support:Buttons

static osThreadId_t tid_thrLED;         // Thread id of thread: LED
static osThreadId_t tid_thrBUT;         // Thread id of thread: BUT

/*------------------------------------------------------------------------------
  thrLED: blink LED
 *----------------------------------------------------------------------------*/
__NO_RETURN void thrLED (void *argument) {
  uint32_t active_flag = 1U;

  for (;;) {
    if (osThreadFlagsWait (1U, osFlagsWaitAny, 0U) == 1U) {
      active_flag ^=1U; 
    }

    if (active_flag == 1U){
			LED_SetOut(15U);														// Switch all 4 LEDs on
			osDelay (500U);															// Delay 500 ms
			unsigned LEDS = 4;													// Where LEDs size is 4
			for(unsigned i = 0; i < LEDS; i++){					
				LED_Off (i);															// Switch current LED off
				osDelay (500U);														// Delay 500 ms
			}
    }
    else {
      osDelay (500U);                             // Delay 500 ms
    }
  }
}

/*------------------------------------------------------------------------------
  thrBUT: check button state
 *----------------------------------------------------------------------------*/
__NO_RETURN static void thrBUT(void *argument) {
  uint32_t last;
  uint32_t state;

  for (;;) {
    state = (Buttons_GetState () & 1U);           // Get pressed button state
    if (state != last){
      if (state == 1){
        osThreadFlagsSet (tid_thrLED, 1U);        // Set flag to thrLED
      }
    }
    last = state;
    osDelay (100U);
  }
}

/*------------------------------------------------------------------------------
 * Application main thread
 *----------------------------------------------------------------------------*/
void app_main (void *argument) {

  LED_Initialize ();                    // Initialize LEDs
  Buttons_Initialize ();                // Initialize Buttons

  tid_thrLED = osThreadNew (thrLED, NULL, NULL);  // Create LED thread
  if (tid_thrLED == NULL) { /* add error handling */ }

  tid_thrBUT = osThreadNew (thrBUT, NULL, NULL);  // Create BUT thread
  if (tid_thrBUT == NULL) { /* add error handling */ }

  osThreadExit();
}
