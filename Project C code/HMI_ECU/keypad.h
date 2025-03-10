/*
 * keypad.h
 *
 *  Created on: 28 May 2024
 *      Author: ahmed
 */

/*******************************************************************************
 *                           Definitions and CONFIGURATIONS		                                  *
 *******************************************************************************/
#ifndef KEYPAD_H_
#define KEYPAD_H_

#include "std_types.h"

/* Use Standard keypad type */
/*#define STANDARD_KEYPAD*/

/* Keypad configurations for number of rows and columns */
#define KEYPAD_NUM_ROWS 			 4
#define KEYPAD_NUM_COLS				 4

/* Keypad Port Configurations */
#define KEYPAD_ROW_PORT_ID 			PORTB_ID
#define FIRST_ROW_PIN 				PIN4_ID

#define KEYPAD_COL_PORT_ID 			PORTD_ID
#define FIRST_COL_PIN 				PIN2_ID

/* Keypad button logic configurations */
/*pull up -> PRESSED = LOGIC_LOW*/
#define KEY_IS_PRESSED 				LOGIC_LOW
#define KEY_IS_RELEASED 			LOGIC_HIGH

#define ASCII_OF_ENTER 				13

#endif /* KEYPAD_H_ */



uint8 KEYPAD_getPressedKey();
