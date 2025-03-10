/*
 * buzzer.c
 *
 *  Created on: 28 Jul 2024
 *      Author: ahmed
 */

#include "gpio.h"
#include "buzzer.h"

#define BUZZER_PORT				PORTD_ID
#define BUZZER_PIN				PIN7_ID

/* Description:
 * Setup the direction for the buzzer pin as output.
 * Turn off the buzzer through the GPIO
 */
void Buzzer_init(void)
{
	GPIO_setupPinDirection(BUZZER_PORT, BUZZER_PIN, PIN_OUTPUT);
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN, LOGIC_LOW);

}

/*enable the Buzzer*/
void Buzzer_on(void)
{
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN, LOGIC_HIGH);
}

/*disable the Buzzer*/
void Buzzer_off(void)
{
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN, LOGIC_LOW);
}
