/*
 * keypad.c
 *
 *  Created on: 28 May 2024
 *      Author: ahmed
 */


#include "common_macros.h"
#include "gpio.h"
#include <avr/delay.h>
#include "keypad.h"

/*******************************************************************************
 *                      Functions Prototypes(Private)                          *
 *******************************************************************************/

#ifndef STANDARD_KEYPAD

	#if (KEYPAD_NUM_COLS == 3)
	/*
	 * Function responsible for mapping the switch number in the keypad to
	 * its corresponding functional number in the proteus for 4x3 keypad
	 */
	static uint8 KEYPAD_4x3_adjustKeyNumber(uint8 button_number);
	#elif (KEYPAD_NUM_COLS == 4)
	/*
	 * Function responsible for mapping the switch number in the keypad to
	 * its corresponding functional number in the proteus for 4x4 keypad
	 */
	static uint8 KEYPAD_4x4_adjustKeyNumber(uint8 button_number);
	#endif

#endif /* STANDARD_KEYPAD */

uint8 KEYPAD_getPressedKey()
{
	uint8 currentRow = 0;
	uint8 currentCol = 0;
	/*Set all ROWs and COLs as input PINS*/
	GPIO_setupPinDirection(KEYPAD_ROW_PORT_ID, FIRST_ROW_PIN, PIN_INPUT);
	GPIO_setupPinDirection(KEYPAD_ROW_PORT_ID, FIRST_ROW_PIN+1, PIN_INPUT);
	GPIO_setupPinDirection(KEYPAD_ROW_PORT_ID, FIRST_ROW_PIN+2, PIN_INPUT);
	GPIO_setupPinDirection(KEYPAD_ROW_PORT_ID, FIRST_ROW_PIN+3, PIN_INPUT);

	GPIO_setupPinDirection(KEYPAD_COL_PORT_ID, FIRST_COL_PIN, PIN_INPUT);
	GPIO_setupPinDirection(KEYPAD_COL_PORT_ID, FIRST_COL_PIN+1, PIN_INPUT);
	GPIO_setupPinDirection(KEYPAD_COL_PORT_ID, FIRST_COL_PIN+2, PIN_INPUT);
#if (KEYPAD_NUM_COLS == 4)
	GPIO_setupPinDirection(KEYPAD_COL_PORT_ID, FIRST_COL_PIN+3, PIN_INPUT);
#endif
	/*make rows output rotationally
	 * check if any COL is PRESSED (LOGIC_LOW) rotationally
	 * */
	while(1)
	{
	for (currentRow = 0; currentRow < KEYPAD_NUM_ROWS; ++currentRow)
	{
		/*set the currentRow as Output*/
		GPIO_setupPinDirection(KEYPAD_ROW_PORT_ID, FIRST_ROW_PIN+currentRow, PIN_OUTPUT);
		/*clear the row output pin*/
		GPIO_writePin(KEYPAD_ROW_PORT_ID, FIRST_ROW_PIN+currentRow,KEY_IS_PRESSED);

		/*loop on the COLs while the one row is output to determine which button is pressed*/
		for (currentCol = 0; currentCol < KEYPAD_NUM_COLS; ++currentCol)
		{
			/*if bit is clear then the button is pressed*/
			if(GPIO_readPin(KEYPAD_COL_PORT_ID, FIRST_COL_PIN+currentCol) == KEY_IS_PRESSED)
			{
				#if (KEYPAD_NUM_COLS == 3)
					#ifdef STANDARD_KEYPAD
						return (currentRow * KEYPAD_NUM_COLS + currentCol+1);
					#else
						return (KEYPAD_4x3_adjustKeyNumber(currentRow * KEYPAD_NUM_COLS + currentCol+1));
					#endif
				#elif (KEYPAD_NUM_COLS == 4)
					#ifdef STANDARD_KEYPAD
						return (currentRow * KEYPAD_NUM_COLS + currentCol+1);
					#else
						return (KEYPAD_4x4_adjustKeyNumber(currentRow * KEYPAD_NUM_COLS + currentCol+1));
					#endif
				#endif

			}
		}
		/*set the currentRow as input again to check the next one*/
		GPIO_setupPinDirection(KEYPAD_ROW_PORT_ID, FIRST_ROW_PIN+currentRow, PIN_INPUT);
		_delay_ms(5); /* Add small delay to fix CPU load issue in proteus */
	}
	}
	}

#if (KEYPAD_NUM_COLS == 3)
	/*
	 * Description :
	 * Update the keypad pressed button value with the correct one in keypad 4x3 shape
	 */
	#ifndef STANDARD_KEYPAD
		static uint8 KEYPAD_4x3_adjustKeyNumber(uint8 button_number)
		{
			uint8 keypad_button = 0;
			switch(button_number)
			{
			case 10:
				keypad_button = '*';
				break;
			case 11:
				keypad_button = 0;
				break;
			case 12:
				keypad_button = '#';
				break;
			default:
				keypad_button = button_number;
				break;
			}
			return keypad_button;
		}
	#endif
#elif (KEYPAD_NUM_COLS == 4)
	/*
	 * Description :
	 * Update the keypad pressed button value with the correct one in keypad 4x4 shape
	 */
	#ifndef STANDARD_KEYPAD
		static uint8 KEYPAD_4x4_adjustKeyNumber(uint8 button_number)
		{
			uint8 keypad_button = 0;
			switch(button_number)
			{
			case 1:
				keypad_button = 7;
				break;
			case 2:
				keypad_button = 8;
				break;
			case 3:
				keypad_button = 9;
				break;
			case 4:
				keypad_button = '/';
				break;
			case 5:
				keypad_button = 4;
				break;
			case 6:
				keypad_button = 5;
				break;
			case 7:
				keypad_button = 6;
				break;
			case 8:
				keypad_button = '*';
				break;
			case 9:
				keypad_button = 1;
				break;
			case 10:
				keypad_button = 2;
				break;
			case 11:
				keypad_button = 3;
				break;
			case 12:
				keypad_button = '-';
				break;
			case 13:
				keypad_button = ASCII_OF_ENTER; /* ASCII of Enter */
				break;
			case 14:
				keypad_button = 0;
				break;
			case 15:
				keypad_button = '=';
				break;
			case 16:
				keypad_button = '+';
				break;
			default:
				keypad_button = button_number;
				break;
			}
			return keypad_button;
		}
	#endif
#endif  /* STANDARD_KEYPAD */
















