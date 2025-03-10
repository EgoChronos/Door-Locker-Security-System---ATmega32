/*
 * timer1.h
 *
 *  Created on: 22 Jul 2024
 *      Author: ahmed
 */

#ifndef TIMER1_OV_CTC_H_
#define TIMER1_OV_CTC_H_

#include "common_macros.h"
#include "std_types.h"

#define SYS_FREQ 				8,000,000

/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/
typedef enum
{
	TIMER1_NO_CLOCK_SOURCE,
	TIMER1_NO_PRESCALING,
	TIMER1_PRESCALER_8,
	TIMER1_PRESCALER_64,
	TIMER1_PRESCALER_256,
	TIMER1_PRESCALER_1024,
}TIMER1_Prescaler;

typedef enum
{
	TIMER1_MODE_NORMAL,
	TIMER1_MODE_CTC,
}TIMER1_Mode;

typedef struct
{
	uint16 initial_value;
	uint16 compare_value; // it will be used in compare mode only.
	TIMER1_Prescaler prescaler;
	TIMER1_Mode mode;
}TIMER1_ConfigType;

/*Initialisation of timer1*/
void Timer1_init(const TIMER1_ConfigType* Config_Ptr);

/*de-init the Timer*/
void Timer1_deInit(void);

/*set callback fcn*/
void Timer1_set_callbackfcn( volatile void (*ptr2fcn) (void));

/*calculates the compare value when you give it the seconds and the pre-scaler value*/
uint16 Timer1_secondsToCompValue(uint8 seconds, uint16 prescalerValue);

#endif /* TIMER1_OV_CTC_H_ */
