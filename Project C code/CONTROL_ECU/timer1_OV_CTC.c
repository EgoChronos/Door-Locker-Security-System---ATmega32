/*
 * timer1.c
 *
 *  Created on: 22 Jul 2024
 *      Author: ahmed
 */

#include <avr/io.h>		/* Include AVR std. library file */
#include <avr/interrupt.h>

#include "common_macros.h"
#include "std_types.h"
#include "timer1_OV_CTC.h"

/*Ptr2fcn carry address of a fcn to excute inside the ISR*/
volatile void (*ptr2callbackFcn) (void);


/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
void Timer1_init(const TIMER1_ConfigType* Config_Ptr)
{
	/* Set timer1 initial count to zero */
	TCNT1 = Config_Ptr-> initial_value;
	/* Set the compare value */
	OCR1A = Config_Ptr-> compare_value;

	/* Configure timer control register TCCR1A
     * Set FOC1A or FOC1B if you want to force compare match
	 */
	/*SET_BIT(TCCR1A, FOC1A);*/

	/* Configure timer control register TCCR1B
	 * 1. Clear WGM12 if Normal mode or Set WGM12 if CTC mode
	 * 2  Enable interrupt for Normal or CTC
	 * 3. Set Pre-scaler
     */
	if (Config_Ptr-> mode == TIMER1_MODE_NORMAL)
	{
		/*choose mode*/
		CLEAR_BIT(TCCR1B, WGM12);

		/*enable Timer1 interrupt*/
		SET_BIT(TIMSK, TOIE1);
	}
	else if (Config_Ptr-> mode == TIMER1_MODE_CTC)
	{
		/*choose mode*/
		SET_BIT(TCCR1B, WGM12);

		/*enable Timer1 interrupt for CTC*/
		SET_BIT(TIMSK, OCIE1A);
	}
	/*set the pre-scaler*/
	TCCR1B = (TCCR1B & 0xF8) | (Config_Ptr-> prescaler & (0x07));
}
/*de-init the Timer*/
void Timer1_deInit(void)
{
		/*disable timer1 clock*/
		CLEAR_BIT(TCCR1B, WGM12);

		/*disable Timer1 interrupt*/
		CLEAR_BIT(TIMSK, TOIE1);
		CLEAR_BIT(TIMSK, OCIE1A);

	/*set the pre-scaler to No clock source*/
	TCCR1B = (TCCR1B & 0xF8) | (TIMER1_NO_CLOCK_SOURCE & (0x07));
}

/*set callback fcn*/
void Timer1_setCallBack( void(*a_ptr)(void))
{
	ptr2callbackFcn = a_ptr;
}

/*calculates the compare value when you give it the seconds and the pre-scaler value*/
uint16 Timer1_secondsToCompValue(uint8 seconds, uint16 prescalerValue)
{
	return (uint16)((float64)seconds*SYS_FREQ/prescalerValue);
}

/*******************************************************************************
 *                                ISR                                          *
 *******************************************************************************/
/*ISR of overflow mode*/
ISR(TIMER1_OVF_vect)
{
	/*call the callbackFcn*/
	(*ptr2callbackFcn)();
}

/*ISR of CTC A mode*/
ISR(TIMER1_COMPA_vect)
{
	/*call the callbackFcn*/
	(*ptr2callbackFcn)();
}
