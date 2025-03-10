/*
 * main.c
 *
 *  Created on: 19 Jul 2024
 *      Author: ahmed
 */
#include "avr/io.h"
#include "avr/delay.h"
#include "uart.h"
#include "dcmotor.h"
#include "timer1_OV_CTC.h"
#include "external_eeprom.h"
#include "buzzer.h"

#define PASS_LENGTH 	 		 		5
/*Communication between ECUs*/
#define READY 							'#'
#define DO_WE_HAVE_PASS 				'$'
#define SAVE_NEW_PASSWORD       		'!'
#define VERIFY_PASSWORD					'?'
#define OPEN_THE_DOOR					'+'
#define CHANGE_PASSWORD 				'-'
#define DOOR_ACTION_DONE 				'%'
#define REMOVE_ERROR_MSG				'*'

/*EEPROM*/
/*Make it true to reset the EEPROM and ERASE THE DEADBEAF then make it FALSE to use the PROGRAM*/
#define ERASE_PASSWORD					FALSE
#define FIRST_TIME 				 		1
#define NOT_FIRST_TIME			 		0
#define FIRST8BITS_ADDRESS				0x0000
#define PASSWORD_TRIAL_COUNT_ADDRESS 	0x0030
#define SAVED_PASSWORD_ADDRESS			0x0050

/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/
/*to count the password trial times*/
typedef enum
{
	TRIAL_SUCCESS,
	TRIAL_ONE,
	TRIAL_TWO,
	PASSWORD_TRIAL_LIMIT,
}TrialCounter;

/*******************************************************************************
 *                        Global Variables                                     *
 *******************************************************************************/


uint8 password[PASS_LENGTH];

/*We have installed password or not*/
boolean havePass = FALSE;
/*to count the password trial times*/
TrialCounter passwordTrialCounter = TRIAL_SUCCESS;
/*carry the verification status of the password*/
boolean passVerified = FALSE;

/*holds configurations of timer1*/
TIMER1_ConfigType timer1Configurations = {0,7812,TIMER1_PRESCALER_1024,TIMER1_MODE_CTC};
uint8 timerInttruptSeconds = 0;
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/* Description:
 * checks if this is the first Time (DEADBEAF)
 */
boolean checkIfFirstTime(void);

/* Description:
 * notify the other ECU that it is READY*/
void sendReady(void);

/* Description:
 * Wait the other ECU to be READY*/
void receiveReady(void);

/* Description:
 * receive request from the HMI_ECU*/
uint8 receiveRequest(void);

boolean saveNewPassword(void);

void verifyPassword(void);

/* Description:
 * 1.rotates the motor of the door 15sec CW to unlock
 * 2.hold for 3sec
 * 3.rotates it 15sec ACW to lock
 */
void openTheDoor(void);

/*this is a callback fcn*/
void timeCounterSec(void);
/*******************************************************************************
*                    	     Main  Function                                    *
 *******************************************************************************/
int main(void)
{
	/*carry the user choice: opening the door or changing the pass*/
	uint8 requestedOption;

	/*initialise the UART*/
	UART_ConfigType UART_configurations = {data8Bits, parityEven, singleStopBit, 9600};
	UART_init(&UART_configurations);

	/*initialise the DC motor and its pins*/
	DcMotor_Init();

	/*initialise the Buzzer*/
	Buzzer_init();

	/*set TIME1 callback fcn*/
	Timer1_setCallBack(timeCounterSec);

	/*enable Global Interrupts*/
	SREG |= (1<<7);


	/*check if this is the first time,
	 * if this is first time then we do not have pass*/
	havePass = !checkIfFirstTime();

#if ERASE_PASSWORD
	uint8 erasePass = 1;
	 /* to setup the program as if you will use it for first time*/
	EEPROM_writeByte(FIRST8BITS_ADDRESS, erasePass);
#endif


	/* if we do not have password,set the passwordTrialCounter to zero and
	 * write it at the EEPROM*/
	if (!havePass)
	{
		passwordTrialCounter = TRIAL_SUCCESS;
		EEPROM_writeByte(PASSWORD_TRIAL_COUNT_ADDRESS, passwordTrialCounter);
	}
	/* else: get the password from the EEPROM and the passwordTrialCounter*/
	else
	{
	uint8 i;
		for (i = 0; i < PASS_LENGTH; ++i)
		{
			EEPROM_readData(SAVED_PASSWORD_ADDRESS, password, PASS_LENGTH);
		}
		EEPROM_readByte(PASSWORD_TRIAL_COUNT_ADDRESS, &passwordTrialCounter);
	}

	/* if passwordTrialCounter equals limit
	 * send it to the HMI_ECU to display threat_MSG
	 * else: send ready and start as normal
	 */
	if (passwordTrialCounter == PASSWORD_TRIAL_LIMIT)
	{
		UART_sendByte(passwordTrialCounter);
	}
	else
	{
		sendReady();
	}

while(1)
	{

		/*If you have tried password too many times then the system is locked for 1 min*/
		if (passwordTrialCounter == PASSWORD_TRIAL_LIMIT)
		{
			/*turn on the buzzer*/
			Buzzer_on();

			/*used to save the number of interrupts which equals the seconds*/
			timerInttruptSeconds = 0;
			/*put system on hold for 1 minute*/
			Timer1_init(&timer1Configurations);
			while (timerInttruptSeconds != 60);
			timerInttruptSeconds = 0;
			Timer1_deInit();

			/*turn off buzzer*/
			Buzzer_off();
			/* send remove the error msg command , set trial counter to zero
			 * and save it to the EEPROM*/
			UART_sendByte(REMOVE_ERROR_MSG);
			passwordTrialCounter = TRIAL_SUCCESS;
			EEPROM_writeByte(PASSWORD_TRIAL_COUNT_ADDRESS, passwordTrialCounter);
		}

		/* Description:
		 * Waits for READY signal from HMI_ECU and responses with READY
		 */
		receiveReady();
		sendReady();
		/*
		 * Let the user choose to:
		 * check if we have password
		 * Open the door
		 * or change the password
		 */
		requestedOption = receiveRequest();

		/*user chooses to open the door*/
		if (requestedOption == DO_WE_HAVE_PASS)
		{
			/*check if we have password*/
			if (havePass)
			{
				UART_sendByte(TRUE);
			}
			else
			{
				UART_sendByte(FALSE);
			}
		}
		else if (requestedOption == SAVE_NEW_PASSWORD)
		{
			/*holds the result of saveNewPass fcn*/
			uint8 passIsSaved = FALSE;
			passIsSaved = saveNewPassword();
			/*if pass is not saved we need to keep it verified to try again*/
			if (!passIsSaved)
			{
				/*Do Nothing*//*keep password verified*/
			}
			else
			{
				/*make password not verified after saving a new password*/
				passVerified = FALSE;
			}
		}
		else if (requestedOption == VERIFY_PASSWORD)
		{
			/* receive password from the (HMI_ECU), check if it is correct
			 * and send the result
			 * */
			verifyPassword();
		}

		else if (requestedOption == OPEN_THE_DOOR)
		{
			if (passVerified)
			{
			openTheDoor();
			/*make password not verified after opening the door*/
			passVerified = FALSE;
			}
		}
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/* Description:
 * checks if this is the first Time
 */
boolean checkIfFirstTime(void)
{
	uint8 i;
	uint8 deadBeaf[8] = {0xD,0xE,0xA,0xD,0xB,0xE,0xA,0xF};
	uint8 first8bitsEEPROM[8];
	EEPROM_readData(FIRST8BITS_ADDRESS, first8bitsEEPROM, 8);

	for (i = 0; i < 8; ++i)
	{
		/*if the value of the first 8 bits != deadbeaf then it is the first time*/
		if (first8bitsEEPROM[i] != deadBeaf[i])
		{
			return FIRST_TIME;
		}
	}

	return NOT_FIRST_TIME;
}
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

/* Description:
 * receive request from the HMI_ECU*/
uint8 receiveRequest(void)
{
	return UART_receiveByte();
}

/* Description:
 * 1.if we do have password and it is not verified, do get new password
 * 2.Receive 2 password entries from the HMI_ECU
 * 3.compare them to know if they match
 * 4.if both are matched save a new password and send passIsSaved TRUE
 * 5.if not send passIsSaved FALSE
 */
boolean saveNewPassword(void)
{
	/*take the first and second entered password and match them*/
	uint8 firstEnteredPass[PASS_LENGTH];
	uint8 secEnteredPass[PASS_LENGTH];
	boolean PasswordsMatched = TRUE;
	boolean newPassSaved = FALSE;
	uint8 passLoopCounter = 0;

	/*if we have password and it is not verified then do not accept new password*/
	if (!passVerified && havePass)
	{
		return FALSE;
	}

	UART_receiveData(firstEnteredPass, PASS_LENGTH);
	UART_receiveData(secEnteredPass, PASS_LENGTH);

	/*match the 2 password entries*/
	for (passLoopCounter = 0; passLoopCounter<PASS_LENGTH; ++passLoopCounter)
	{
		if (firstEnteredPass[passLoopCounter] != secEnteredPass[passLoopCounter])
		{
			PasswordsMatched = FALSE;
		}
	}

	/*if matched save it as new password
	 * send newPassSaved TRUE
	 * make havePass TRUE
	 * if not send newPassSaved False
	 * */
	if (PasswordsMatched)
	{
		for (passLoopCounter = 0; passLoopCounter < PASS_LENGTH; ++passLoopCounter)
		{
			password[passLoopCounter] = firstEnteredPass[passLoopCounter];
		}


			uint8 deadBeaf[8] = {0xD,0xE,0xA,0xD,0xB,0xE,0xA,0xF};
			/*write deadbeaf at the first 8 bits of EEPROM*/
			EEPROM_writeData(FIRST8BITS_ADDRESS, deadBeaf, 8);

		EEPROM_writeData(SAVED_PASSWORD_ADDRESS, password, PASS_LENGTH);
		newPassSaved = TRUE;
		havePass = TRUE;
		UART_sendByte(newPassSaved);
	}
	else
	{
		UART_sendByte(newPassSaved);
	}
	return newPassSaved;
}

/* Description:
 * 1.send the password trial counter to the HMI_ECU
 * 2.try until password is right or reach the trial limit
 */
void verifyPassword(void)
{
	/*carries the verification result*/
	boolean passwordIsCorrect = TRUE;
	uint8 enteredPassword[PASS_LENGTH];
	uint8 passLoopCounter = 0;
	/*send password trial counter*/
	UART_sendByte(passwordTrialCounter);
	/*try until you enter the right password or reach the trial limit*/
	while (passwordTrialCounter < PASSWORD_TRIAL_LIMIT)
	{
	passwordIsCorrect = TRUE;
		UART_receiveData(enteredPassword, PASS_LENGTH);

		for (passLoopCounter = 0; passLoopCounter < PASS_LENGTH; ++passLoopCounter)
		{
			if(enteredPassword[passLoopCounter] != password[passLoopCounter])
			{
				passwordIsCorrect = FALSE;
			}
		}

		if (passwordIsCorrect)
		{
			passVerified = TRUE;
			passwordTrialCounter = TRIAL_SUCCESS;
			EEPROM_writeByte(PASSWORD_TRIAL_COUNT_ADDRESS, passwordTrialCounter);
			UART_sendByte(passwordTrialCounter);
			break;
		}
		else
		{
			++passwordTrialCounter;
			EEPROM_writeByte(PASSWORD_TRIAL_COUNT_ADDRESS, passwordTrialCounter);

			UART_sendByte(passwordTrialCounter);
		}
	}
}

/* Description:
 * 1.rotates the motor of the door CW to unlock
 * 2.set a timer1 interrupt 15sec which then hold the door
 */

void openTheDoor(void)
{
	DcMotor_rotate(DC_Motor_CW);
	Timer1_init(&timer1Configurations);
	while (timerInttruptSeconds != 15);

	DcMotor_off();
	while (timerInttruptSeconds != 18);

	UART_sendByte(DOOR_ACTION_DONE);
	DcMotor_rotate(DC_MOTOR_ACW);
	while (timerInttruptSeconds != 32);
	Timer1_deInit();
	timerInttruptSeconds = 0;
	DcMotor_off();
	UART_sendByte(DOOR_ACTION_DONE);
}
/*this is a callback fcn*/
void timeCounterSec(void)
{
	++timerInttruptSeconds;
}
