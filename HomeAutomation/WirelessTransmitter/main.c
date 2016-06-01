/*
 * Wireless Transmitter
 *
 * Created: 12/12/2014 6:42:48 p.m.
 * Author: Tick
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "nrf24.h"

#include <avr/power.h>

//TODO: assign_unit_address(uint8_t address)
//Function which stores assigned identifier into EEPROM

//TODO: status_change()
//Function which updates unit status on message receive

//TODO: Set ALL pins to OUTPUT mode and OFF to save power
//TODO: Disable ADC

#define DEBUG_LED (PORTB)
#define DEBUG_LED_PIN (1<<0)
#define INPUT_SWITCH (PINC)
#define INPUT_SWITCH_PIN (1<<2)
#define INPUT_SWITCH_PIN2 (1<<3)

int main(void)
{

	clock_prescale_set(clock_div_1);

	nrf24_init();
	nrf24_config(1,1);	//Channel, payload length

	//TODO: Accommodate for protocol [address][payload][???]
	uint8_t command_array[1];	//Create container for commands

	uint8_t tx_address[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
	uint8_t tx_address2[5] = {0xE8,0xE8,0xE8,0xE8,0xE8};
	nrf24_tx_address(tx_address);

	uint8_t rx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};
	uint8_t rx_address2[5] = {0xD7,0xD7,0xD7,0xD7,0xD8};

	nrf24_rx_address(rx_address);
	
	uint8_t switch_state, old_switch_state;
	
	DDRB |= (DEBUG_LED_PIN);				//Set B1 to output for dbg LED
	DEBUG_LED &= ~DEBUG_LED_PIN;			//Turn off dbg LED

	DDRC &= ~(INPUT_SWITCH_PIN | INPUT_SWITCH_PIN2);			//Set C2 to input
	PORTC |= (INPUT_SWITCH_PIN | INPUT_SWITCH_PIN2);			//Set pullup resistor for C2


	
	//Perform a double blink to indicate it is working
	DEBUG_LED |= (DEBUG_LED_PIN);
	_delay_ms(500);
	DEBUG_LED &= ~(DEBUG_LED_PIN);
	_delay_ms(500);
	DEBUG_LED |= (DEBUG_LED_PIN);
	_delay_ms(500);
	DEBUG_LED &= ~(DEBUG_LED_PIN);
	//======

	switch_state = (INPUT_SWITCH & (INPUT_SWITCH_PIN | INPUT_SWITCH_PIN2));			//Read PinD0
	old_switch_state = switch_state;

	uint8_t state = 0;
	uint8_t* module_address = 0;

    while(1)
    {
    	_delay_ms(1);

    	switch_state = (INPUT_SWITCH & (INPUT_SWITCH_PIN | INPUT_SWITCH_PIN2));	//Read the switch state

    	if( (switch_state ^ INPUT_SWITCH_PIN) & INPUT_SWITCH_PIN)
    	{
    		module_address = tx_address;
    		//
    	}

    	if( (switch_state ^ INPUT_SWITCH_PIN2) & INPUT_SWITCH_PIN2)
    	{
    		module_address = tx_address2;
    	}


		if ( switch_state != old_switch_state )
		{
			old_switch_state = switch_state;
			switch( state )
			{
				case 0:
					state++;
					{

						nrf24_tx_address(module_address);
						//nrf24_rx_address(module_address);
						command_array[0] = 1;
						nrf24_send(command_array);	//Send switch state to turn on/off light
						while(nrf24_isSending())	//Wait for shit to send
						{
							DEBUG_LED |= (DEBUG_LED_PIN);	//Turn dbg LED ON
						}
						if(nrf24_lastMessageStatus()==NRF24_TRANSMISSON_OK)
						{
							DEBUG_LED &= ~(DEBUG_LED_PIN);	//Turn dbg LED OFF
						}

					}

					break;

				case 1:
					DEBUG_LED &= ~(DEBUG_LED_PIN);	//Turn dbg LED OFF
					state = 0;
					break;

			}
		}


    }
	_delay_ms(500);

	return 0;
}



