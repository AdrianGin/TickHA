/*
 * Wireless Receiver
 *
 * Created: 12/12/2014 6:42:48 p.m.
 * Author: Tick
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "nrf24.h"

//===================
// Node Addressing
//===================
// These should be called only after an interrupt is called (on wireless receive)
void set_nodeAddress(char c_nodeAddress)
{
	// Wait for completion of previous write
	while(EECR & (1<<EEPE));
	
	// Set up address and Data Registers
	EEAR = 0;				// set EEPROM address to 0 (can be 0-4096)
	EEDR = c_nodeAddress;	// load data into the EEPROM data register
	
	// Write logical one to EEMPE
	EECR |= (1<<EEMPE);		// enable writing to EEPROM
	// Initiate EEPROM write by setting EEPE
	EECR |= (1<<EEPE);		// send logical 1 to initiate write to EEPROM
}

char read_nodeAddress()
{
	// Wait for completion of previous write
	while(EECR & (1<<EEPE));
	
	//Set up address register
	EEAR = 0;				// node address is stored in location 0 of EEPROM
	// Start reading from EEPROM by writing logical one to EERE
	EECR |= (1<<EERE);
	
	return EEDR;			// Return read data from Data Register
}

//TODO: update_controller()
//Function which updates controller about status change (manual override)

//TODO: Set ALL pins to OUTPUT mode and OFF to save power
//TODO: Disable ADC


#define DEBUG_LED (PORTB)
#define DEBUG_LED_PIN (1<<1)
#define INPUT_SWITCH (PINC)
#define INPUT_SWITCH_PIN (1<<2)

int main(void)
{
	nrf24_init();
	nrf24_config(1,1);	//Channel, payload length
	
	//TODO: Accommodate for protocol [address][payload][???]
	uint8_t data[1];	//Create data container for incoming commands
	data[0] = 0;
	
	uint8_t tx_address[5] = {0xD8,0xD8,0xD8,0xD8,0xD8};
	nrf24_tx_address(tx_address);

	uint8_t rx_address[5] = {0xE8,0xE8,0xE8,0xE8,0xE8};
	nrf24_rx_address(rx_address);
	
	DDRB |= (DEBUG_LED_PIN);				//Set B1 to output for dbg LED
	DEBUG_LED &= ~DEBUG_LED_PIN;			//Turn off dbg LED

	//Perform a double blink to indicate it is working
	DEBUG_LED |= (DEBUG_LED_PIN);
	_delay_ms(500);
	DEBUG_LED &= ~(DEBUG_LED_PIN);
	_delay_ms(500);
	DEBUG_LED |= (DEBUG_LED_PIN);
	_delay_ms(500);
	DEBUG_LED &= ~(DEBUG_LED_PIN);
	//======
	
	while(1)
	{
		if(nrf24_dataReady())
		{
			nrf24_getData(data);
			DEBUG_LED ^= (DEBUG_LED_PIN);
		}
	}
}
