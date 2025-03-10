/*
 * buzzer.h
 *
 *  Created on: 28 Jul 2024
 *      Author: ahmed
 */

#ifndef BUZZER_H_
#define BUZZER_H_

/* Description:
 * Setup the direction for the buzzer pin as output.
 * Turn off the buzzer through the GPIO
 */
void Buzzer_init(void);

/*enable the Buzzer*/
void Buzzer_on(void);

/*disable the Buzzer*/
void Buzzer_off(void);

#endif /* BUZZER_H_ */
