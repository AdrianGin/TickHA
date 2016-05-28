/* Serial Port routines for the AVR mpu's.
*  These functions utilise the hardware USART0 registers
*
* Filename: harUart.c
* By Adrian Gin (amg94@student.canterbury.ac.nz)
* Created: 16-06-06
* 
* For AVR USART0 Core
* Since the UART has true ouputs, a MAX232 is required to interface
* with a computer's serial port.
*/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include	<util/delay.h>
#include	"UART.h"
/* uartInit:
 * Initialises the baudrate, parity, stop bit generation and 8bit mode
 * It must be called before any hardUart function is used 
 *
 * Interrupts are not set by default.
 *
 */
void uartInit(uint8_t baudrate, uint8_t U2Xvalue)
{
	/*Setup the U2X Bit*/
	UCSR0A	=	(UCSR0A & (~(1<<U2X0))) | (U2Xvalue << U2X0);
	
	UCSR0B |= (1<<RXEN0) | (1<<TXEN0);	/*Enable Rx and Tx modules*/
	UCSR0B &= ~(1<<UCSZ02);				/*Set to 8bit mode*/
	

	/*Select UCSRC to be written to*/	
	/* Set to Asynchronous Mode
	 *			 1 Stop-bit
	 *			 No Parity
	 *			 8-bit char mode
	 */
	UCSR0C = (UCSR0C & ~( UCSRCMASK )) | (NOPARITY<<UPM00) | (BIT8 << UCSZ00);

	/*Set the baud rate to the desired rate*/
	uartSetBaud(baudrate, 0);
}

/* uartSetBaud:
 * Changes the baudrate to the specified value.
 * See the datasheet for more details on what the
 * Baudrate generation registers should be.
 */
void uartSetBaud(uint8_t baudrateL, uint8_t baudrateH)
{
	UBRR0H = 	baudrateH;
	/* The lower 8bits must be written last as the baudrate generator
	 * is updated as soon as UBRRL is written to*/
	UBRR0L	=	baudrateL;

}


/* Disables the Receiver and Transmitter modules*/
void uartDisable(void)
{
	UCSR0B &= ~((1<<RXEN0) | (1<<TXEN0));	/*Disable Rx and Tx modules*/
	
}

/* uartTx:
 *
 * Transmits the passed byte to the Uart.Tx pin.
 *
 */
void uartTx(uint8_t outbyte)
{
	/*Wait until output shift register is empty*/	
	while( (UCSR0A & (1<<UDRE0)) == 0 );
		
	/*Send byte to output buffer*/
	UDR0	= outbyte;
}

/** Writes nbytes of buffer to the UART */
void uartTxDump(uint8_t* buffer, uint8_t nbytes )
{
	uint16_t i = 0;
	while( i++ < nbytes )
	{
		uartTx(*buffer++);
	}
}


/* uartTxString:
 * Outputs the passed string to the UART.Tx pin
 * The output is true ouput, not inverted, so a MAX232 or some sort of
 * TTL -> +/- 15V converter is required.
 */
void uartTxString(uint8_t* outString)
{
	while( *outString )
	{
		uartTx(*outString++);
   }
	
}

/* Usage: uartTxString_P( PSTR("hello!") ); */
/* Send a string which resides in the program memory */
void uartTxString_P(const char* outString_P)
{

   char c;

   while( (c = pgm_read_byte(outString_P++)) )
   {
      uartTx(c);    
   }
}

void uartNewLine(void)
{
   uartTx('\r');
   uartTx('\n'); 
}

/* To echo the receiver buffer, write this code in the main.c file */
/*
ISR(SIG_UART_RECV)
{
	uartTx(UDR);
}
*/

