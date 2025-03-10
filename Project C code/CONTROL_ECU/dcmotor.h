/*
 * dcmotor.h
 *
 *  Created on: 19 Jun 2024
 *      Author: ahmed
 */

#ifndef DCMOTOR_H_
#define DCMOTOR_H_

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define DC_MOTOR_PORT 					PORTC_ID
#define DC_MOTOR_RIGHTHAND_PIN 			PIN6_ID
#define DC_MOTOR_LEFTHAND_PIN 			PIN2_ID
/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/

/*the states of any dc motor*/
typedef enum
{
	DC_Motor_CW, DC_MOTOR_ACW,
}DcMotor_rotationDirection;


/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/
/* Description
 * setup the direction of the two motor pins
 * turn the motor off at the start
 */
void DcMotor_Init(void);

/* Description:
 * turn of the DC motor*/
void DcMotor_off(void);

/* Description:
 * Rotates the DC motor to the ACW or CW
 */
void DcMotor_rotate(DcMotor_rotationDirection rotationDirection);

#endif /* DCMOTOR_H_ */
