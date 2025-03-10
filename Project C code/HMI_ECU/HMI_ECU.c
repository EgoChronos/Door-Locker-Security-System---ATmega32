/*
 * main.c
 *
 *  Created on: 19 Jul 2024
 *      Author: ahmed
 */


#include "lcd.h"
#include "keypad.h"
#include "avr/delay.h"
#include "uart.h"

#define PASS_LENGTH 	 		 5
/*Communication between ECUs*/
#define READY 					'#'
#define DO_WE_HAVE_PASS 		'$'
#define SAVE_NEW_PASSWORD       '!'
#define VERIFY_PASSWORD			'?'
#define OPEN_THE_DOOR			'+'
#define CHANGE_PASSWORD 		'-'
#define DOOR_ACTION_DONE 		'%'
#define REMOVE_ERROR_MSG		'*'

/*to count the password trial times*/
typedef enum
{
	SUCCESS,
	ONE,
	TWO,
	PASSWORD_TRIAL_LIMIT,
}TrialCounter;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/* Description:
 * notify the other ECU that it is READY*/
void sendReady(void);

/* Description:
 * Wait the other ECU to be READY*/
void receiveReady(void);

/* Description
 * 1.Check the CONTROL_ECU if we have installed password
 */
boolean doWeHavePassword(void);
/*Description:
 * Asks the user to enter/re-enter the pass
 * configure isFirstpass argument as TRUE/FALSE to enter/re-enter the pass
 * Receive 5 characters
 * Return them on the string passed to the fcn when the user press enter
 */
void askUserAndTakePassword (uint8* a_enteredPass, boolean FirstTime);

/* Description:
 * ask the user for pass of 5 characters
 * ask the user to re-enter the password
 * send it to the CONTROL_ECU to save it
 */
boolean takeNewPassword(void);

/* Description:
 * 1.Display Open the door and change password options on the LCD
 * 2.get the option from the user through the keypad
 */
uint8 mainOptions(void);

/* Description:
 * sends the request to the CONTROL_ECU
 * asking the suer to enter a pass to open the door
 * verify it
 * if correct shows opening msg on LCD if not try again
 * if exceeds the trial limit calls the error fcn
 */
boolean verifyPassword(void);

/* Description:
 * asking the suer to enter a pass to open the door
 * verify it
 * if correct shows opening msg on LCD if not try again
 * if exceeds the trial limit calls the error fcn
 */
void openTheDoor(void);

/* Description:
 * 1.Ask suer to enter pass
 * 2.Send the entered password to the CONTROL_ECU to verify it
 * 3.receive the result (success = 0, or number of trials) and return it
 */
TrialCounter takePassAndSendToVerify(void);

/* Description:
 * shows error msg and stop user from entering anything for 1 min
 */
void threatErrorMSG(void);

/*******************************************************************************
 *                    	     Main  Function                                    *
 *******************************************************************************/
int main(void)
{
	/*carry the user choice: opening the door or changing the pass*/
	volatile uint8 userOptionChoice;

	/*initialise the LCD*/
	LCD_init();
	/*initialise the UART*/
	UART_ConfigType UART_configurations = {data8Bits, parityEven, singleStopBit, 9600};
	UART_init(&UART_configurations);

	if (UART_receiveByte() == PASSWORD_TRIAL_LIMIT)
	{
		threatErrorMSG();
	}
while(1)
	{
		/*carry the CONTROL_ECU response: we have password or not*/
		boolean havePass = TRUE;

		/*carry the CONTROL_ECU response: password changed successfully or not*/
		boolean newPassSaved = FALSE;

		/* Description
		 * 1.Send Ready and receive it From CONCTROL_ECU
		 * 2.Check the CONTROL_ECU if we have installed password
		 * 3.if not then change password, else continue
		 */
		sendReady();
		receiveReady();

		/* Description
		 * 1.Check the CONTROL_ECU if we have installed password
		 * 2.if not then change password, else continue
		 */
		havePass = doWeHavePassword();

		/* if we do not have password install then get a password
			 * if pass not changed successfully ask user for pass again*/
		if (!havePass)
		{
			while (!newPassSaved)
			{
				newPassSaved = takeNewPassword();
			}
		}

		/* LCD usual display
		 * Let the user choose to:
		 * Open the door
		 * or change the password
		 */
		userOptionChoice = mainOptions();

		/*user chooses to open the door*/
		if (userOptionChoice == OPEN_THE_DOOR)
		{
			/*saves the verification result*/
			boolean passVerificationResult = FALSE;
			/* Ask user for password and verify it giving him limited tries
			 * return result*/
			passVerificationResult = verifyPassword();
			if (passVerificationResult)
			{
				openTheDoor();
			}
			else
			{
			/* shows error msg and stop user from entering anything for 1 min*/
			threatErrorMSG();
			}
		}

		else if (userOptionChoice == CHANGE_PASSWORD)
		{
			/*saves the verification result*/
			boolean passVerificationResult = FALSE;
			/* Ask user for password and verify it giving him limited tries
			 * return result*/
			passVerificationResult = verifyPassword();
			if (passVerificationResult)
			{
				takeNewPassword();
			}
			else
			{
			/* shows error msg and stop user from entering anything for 1 min*/
			threatErrorMSG();
			}
		}
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/* Description:
 * notify the other ECU that it is READY*/
void sendReady(void)
{
	/*Sends READY to the Other ECU*/
	UART_sendByte(READY);
}
/* Description:
 * Wait the other ECU to be READY*/
void receiveReady(void)
{	/*receive READY from the other ECU*/
	while(UART_receiveByte() != READY);
}

/* Description
 * 1.Check the CONTROL_ECU if we have installed password
 */
boolean doWeHavePassword(void)
{
	/*carry the CONTROL_ECU response: we have installed password or not*/
	boolean havePass = FALSE;

	/*Asks the CONTROL_ECU if we have password installed*/
	UART_sendByte(DO_WE_HAVE_PASS);
	havePass = UART_receiveByte();

	return havePass;
}
/*Description:
 * Asks the user to enter/re-enter the pass
 * configure isFirstpass argument as TRUE/FALSE to enter/re-enter the pass
 * Receive 5 characters
 * Return them on the string passed to the fcn when the user press enter
 */
void askUserAndTakePassword (uint8* a_enteredPass, boolean FirstTime)
{

	/*counter to receive password of 5 characters*/
	uint8 passCounter = 0;

	LCD_clearScreen();
	/*Display msg to enter or re-enter the pass in the first line*/
	if (FirstTime)
	{
	LCD_displaystringRowColumn(0,0,(uint8*)"Plz enter pass:");
	}
	else
	{
	LCD_displaystringRowColumn(0,0,(uint8*)"Plz re-enter the");
	}

	/*get the password from user and type * for each character on the LCD and save it*/
	LCD_moveCursor(1,0);

	/*if this is the second time display (same pass:) on the LCD:*/
	if (!FirstTime)
	{
		LCD_displayString((uint8*)"same Pass: ");
	}

	/*delay before you get any character*/
	for (passCounter = 0; passCounter < PASS_LENGTH; ++passCounter)
	{
	 _delay_ms(250);
	 a_enteredPass[passCounter] = KEYPAD_getPressedKey();
	 _delay_ms(250);
	LCD_displayCharacter('*');
	}

	/*Do not get out until the user press Enter (ON/c) button*/
	while (KEYPAD_getPressedKey() != ASCII_OF_ENTER);
}

/* Description:
 * ask the user for pass of 5 characters
 * ask the user to re-enter the password
 * send it to the CONTROL_ECU to save it
 */
boolean takeNewPassword(void)
{
	sendReady();
	receiveReady();

	UART_sendByte(SAVE_NEW_PASSWORD);

	/*The pass the user entered saved here*/
	uint8 firstEnteredPass[PASS_LENGTH];
	uint8 secEnteredPass[PASS_LENGTH];
	boolean newPassSaved = FALSE;

	/*Description:
		 * Asks the user to enter the pass
		 * configure FirstTime argument as TRUE to enter
		 * Receive 5 characters
		 * Return them on the string passed to the fcn when the user press enter
		 */
		LCD_clearScreen();
		askUserAndTakePassword(firstEnteredPass, TRUE);
		UART_sendData(firstEnteredPass, PASS_LENGTH);
		/*clean the screen to re-enter the pass*/

		/*Description:
		 * Asks the user to re-enter the pass
		 * configure FirstTime argument as False to re-enter
		 * Receive 5 characters
		 * Return them on the string passed to the fcn when the user press enter
		 */
		LCD_clearScreen();
		askUserAndTakePassword(secEnteredPass, FALSE);
		UART_sendData(secEnteredPass, PASS_LENGTH);

		newPassSaved = UART_receiveByte();

		if (newPassSaved == TRUE)
		{
			LCD_clearScreen();
			LCD_displayString((uint8*)"Pass changed");
			LCD_displaystringRowColumn(1,0,(uint8*)"Successfully");
			_delay_ms(500);
		}
		else
		{
			LCD_clearScreen();
			LCD_displayString((uint8*)"Try Again");
			_delay_ms(500);
		}
	return newPassSaved;
}

/* Description:
 * 1.Display Open the door and change password options on the LCD
 * 2.get the option from the user through the keypad
 */
uint8 mainOptions(void)
{
	/*phrases to display on the LCD*/
	LCD_clearScreen();
	LCD_displaystringRowColumn(0,0,(uint8*)"+ : Open Door");
	LCD_displaystringRowColumn(1,0,(uint8*)"- : Change Pass");

	return KEYPAD_getPressedKey();
}

/* Description:
 * sends the request to the CONTROL_ECU
 * asking the suer to enter a pass to open the door
 * verify it
 * if correct shows opening msg on LCD if not try again
 * if exceeds the trial limit calls the error fcn
 */
boolean verifyPassword(void)
{
	/*number of times we entered the password wrong*/
	TrialCounter passwordTrialCounter = SUCCESS;
	/*return of the verification result*/
	boolean passwordVerified = FALSE;

	/*send the request to the CONTROL_ECU*/
	sendReady();
	receiveReady();
	UART_sendByte(VERIFY_PASSWORD);

	/*receive how many times the password was tried until now*/
	passwordTrialCounter = UART_receiveByte();

	/*try until you enter the right pass word or reach the trial limit*/
	while (passwordTrialCounter < PASSWORD_TRIAL_LIMIT)
	{
		/*get the trial counter*/
		passwordTrialCounter = takePassAndSendToVerify();

		/*if password is correct then make verification result TRUE*/
		if (passwordTrialCounter == SUCCESS)
		{
			passwordVerified = TRUE;
			return passwordVerified;
		}
	}
	return passwordVerified;
}

/* Description:
 * 1.Ask suer to enter pass
 * 2.Send the entered password to the CONTROL_ECU to verify it
 * 3.receive the result (success = 0, or number of trials) and return it
 */
TrialCounter takePassAndSendToVerify(void)
{
	/*entered password from the user*/
	uint8 enteredPassword[PASS_LENGTH];

	/*get the password form the user*/
	askUserAndTakePassword(enteredPassword, TRUE);
	/*send the password to CONTROL_ECU to verify it*/
	UART_sendData(enteredPassword, PASS_LENGTH);
	/*receive the result (number of trials, if 0 then means success) and return it*/
	return UART_receiveByte();
}

void openTheDoor(void)
{
	/* Send Ready and receive it From CONCTROL_ECU */
	sendReady();
	receiveReady();

	/*send the request to the CONTROL_ECU*/
	UART_sendByte(OPEN_THE_DOOR);

	/* Displaying opening the door msg on the screen using the timer*/
	LCD_clearScreen();
	LCD_displayString((uint8*)"Door is");
	LCD_displaystringRowColumn(1,4,(uint8*)"Unlocking");
	while (UART_receiveByte() != DOOR_ACTION_DONE);

	LCD_clearScreen();
	LCD_displayString((uint8*)"Door is locking");
	while (UART_receiveByte() != DOOR_ACTION_DONE);
}

/* Description:
 * shows error msg and stop user from entering anything for 1 min
 */
void threatErrorMSG(void)
{
	LCD_clearScreen();
	LCD_displayString((uint8*)"ERROR");
	LCD_displaystringRowColumn(1,0,(uint8*)"IS THIS UR HOME?");
	while (UART_receiveByte() != REMOVE_ERROR_MSG);
}

