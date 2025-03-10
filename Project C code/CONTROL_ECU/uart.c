

#include "uart.h"
#include "common_macros.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType* Config_Ptr)
{
	uint16 ubrr_value = 0;
	/*clean all registers*/
	UCSRA = 0;
	UCSRB = 0;
	UCSRC = 0;

	/*U2X = 1 for Double Transmission Speed Enabled*/
	SET_BIT(UCSRA, U2X);

	/************************** UCSRB Description **************************
	 * RXCIE = 0 Disable USART RX Complete Interrupt Enable
	 * TXCIE = 0 Disable USART Tx Complete Interrupt Enable
	 * UDRIE = 0 Disable USART Data Register Empty Interrupt Enable
	 * RXEN  = 1 Receiver Enable
	 * RXEN  = 1 Transmitter Enable
	 * UCSZ2 = 0 For 8-bit data mode
	 * RXB8 & TXB8 not used for 8-bit data mode
	 ***********************************************************************/
	SET_BIT(UCSRB, RXEN);
	SET_BIT(UCSRB, TXEN);

	/************************** UCSRC Description **************************
	 * URSEL   = 1 The URSEL must be one when writing the UCSRC
	 * UMSEL   = 0 Asynchronous Operation
	 * UPM1:0  = 00 Disable parity bit
	 * USBS    = 0 One stop bit
	 * UCSZ1:0 = 11 For 8-bit data mode
	 * UCPOL   = 0 Used with the Synchronous operation only
	 ***********************************************************************/

	if (Config_Ptr->parity == parityDisabled)
	{
		CLEAR_BIT(UCSRC, UPM0);
		CLEAR_BIT(UCSRC, UPM1);
	}
	else if (Config_Ptr->parity == parityEven)
	{
		CLEAR_BIT(UCSRC, UPM0);
		SET_BIT(UCSRC, UPM1);
	}
	else if (Config_Ptr->parity == parityOdd)
	{
		SET_BIT(UCSRC, UPM0);
		SET_BIT(UCSRC, UPM1);
	}
	SET_BIT(UCSRC, URSEL);

	if (Config_Ptr->stop_bit == DoubleStopBits)
	{
	SET_BIT(UCSRC, USBS);
	}

	switch(Config_Ptr->bit_data)
	{
		case data5Bits:
			CLEAR_BIT(UCSRC, UCSZ0);
			CLEAR_BIT(UCSRC, UCSZ1);
			CLEAR_BIT(UCSRC, UCSZ2);
			break;
		case data6Bits:
			SET_BIT(UCSRC, UCSZ0);
			CLEAR_BIT(UCSRC, UCSZ1);
			CLEAR_BIT(UCSRC, UCSZ2);
			break;
		case data7Bits:
			CLEAR_BIT(UCSRC, UCSZ0);
			SET_BIT(UCSRC, UCSZ1);
			CLEAR_BIT(UCSRC, UCSZ2);
			break;
		case data8Bits:
			SET_BIT(UCSRC, UCSZ0);
			SET_BIT(UCSRC, UCSZ1);
			CLEAR_BIT(UCSRC, UCSZ2);
			break;
	}

	/* Calculate the UBRR register value */
	ubrr_value = (uint16)((F_CPU / (Config_Ptr->baud_rate * 8UL)) - 1);

	/************************** UBRRH Description **************************
	 * URSEL   = 0 The URSEL must be Zero when writing the UBRRH
	 * First 8 bits from the BAUD_PRESCALE inside UBRRL and last 4 bits in UBRRH
	 */
	CLEAR_BIT(UBRRH, URSEL);
	UBRRH = (ubrr_value>>8);
	UBRRL = ubrr_value;

}

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data)
{

	/*
	 * UDRE flag is set when the Tx buffer (UDR) is empty and ready for
	 * transmitting a new byte so wait until this flag is set to one
	 */
	while (BIT_IS_CLEAR(UCSRA, UDRE));
	/*
	 * Put the required data in the UDR register and it also clear the UDRE flag as
	 * the UDR register is not empty now
	 */
	UDR = data;
}

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_receiveByte(void)
{
	/* RXC flag is set when the UART receive data so wait until this flag is set to one */
	while (BIT_IS_CLEAR(UCSRA, RXC));
	/*
	 * Read the received data from the Rx buffer (UDR)
	 * The RXC flag will be cleared after read the data
	 */
	return UDR;
}

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8* str)
{
	uint8 i = 0;
	while (str[i] != '\0')
	{
		UART_sendByte(str[i]);
		++i;
	}
}

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *str)
{
	uint8 i = 0;

	/* Receive the first byte */
	str[i] = UART_receiveByte();

	/* Receive the whole string until the '#' */
	while (str[i] != '#')
	{
		++i;
		str[i] = UART_receiveByte();
	}

	/* After receiving the whole string plus the '#', replace the '#' with '\0' */
	str[i] = '\0';
}

/*
 * Description :
 * Send the required data with the given length through UART to the other UART device.
 */
void UART_sendData(const uint8* data, uint8 length)
{
	uint8 i = 0;
	for (i = 0; i < length; ++i)
	{
		UART_sendByte(data[i]);
	}
}

/*
 * Description :
 * Receive the required data with the given length through UART from the other UART device.
 */
void UART_receiveData(uint8* data, uint8 length)
{
	uint8 i = 0;

	/* Receive the whole string until the '#' */
	for (i = 0; i < length; ++i)
	{
		data[i] = UART_receiveByte();
	}

}





