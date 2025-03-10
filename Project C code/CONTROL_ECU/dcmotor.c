/*
 * dcmotor.c
 *
 *  Created on: 19 Jun 2024
 *      Author: ahmed
 */
#include "gpio.h"
#include "dcmotor.h"
#include <avr/delay.h>

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/* Description
 * setup the direction of the two motor pins
 * turn the motor off at the start
 */
void DcMotor_Init(void)
{
	/*setup the 2 motor pins direction as output pins*/
	GPIO_setupPinDirection(DC_MOTOR_PORT, DC_MOTOR_RIGHTHAND_PIN, PIN_OUTPUT);
	GPIO_setupPinDirection(DC_MOTOR_PORT, DC_MOTOR_LEFTHAND_PIN, PIN_OUTPUT);

	/*Turn the motor off at the start by setting both pins to logic low*/
	GPIO_writePin(DC_MOTOR_PORT, DC_MOTOR_RIGHTHAND_PIN, LOGIC_LOW);
	GPIO_writePin(DC_MOTOR_PORT, DC_MOTOR_LEFTHAND_PIN, LOGIC_LOW);

}

/* Description:
 * turn of the DC motor*/
void DcMotor_off(void)
{
	/*Turn the motor off at the start by setting both pins to logic low*/
	GPIO_writePin(DC_MOTOR_PORT, DC_MOTOR_RIGHTHAND_PIN, LOGIC_LOW);
	GPIO_writePin(DC_MOTOR_PORT, DC_MOTOR_LEFTHAND_PIN, LOGIC_LOW);
}

/* Description:
 * Rotates the DC motor to the ACW or CW
 */
void DcMotor_rotate(DcMotor_rotationDirection rotationDirection)
{
	_delay_ms(20);
	if (rotationDirection == DC_MOTOR_ACW)
	{
		/*Make the right hand high and left hand low to rotate ACW*/
		GPIO_writePin(DC_MOTOR_PORT, DC_MOTOR_LEFTHAND_PIN, LOGIC_LOW);
		GPIO_writePin(DC_MOTOR_PORT, DC_MOTOR_RIGHTHAND_PIN, LOGIC_HIGH);
	}
	else if (rotationDirection == DC_Motor_CW)
	{
		/*Make the right hand high and left hand low to rotate ACW*/
		GPIO_writePin(DC_MOTOR_PORT, DC_MOTOR_RIGHTHAND_PIN, LOGIC_LOW);
		GPIO_writePin(DC_MOTOR_PORT, DC_MOTOR_LEFTHAND_PIN, LOGIC_HIGH);
	}
}
