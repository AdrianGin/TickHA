

#include <stdint.h>
#include <util/delay.h>

typedef struct AM2302
{
	volatile uint8_t* pin_dir_reg; //eg. DDRC
	volatile uint8_t* pin_out_reg; //eg. PORTC
	volatile uint8_t* pin_in_reg;  //eg. PINC
	volatile uint8_t  pin_index;   //eg. PC6

	uint16_t humidity;
	uint16_t temperature;


} AM2302_t;


typedef struct AM2302_Temp
{
	uint8_t sign;
	uint8_t integer;
	uint8_t decimal;

} AM2302_Temp_t;



#define AM2302_Delay_us(us) _delay_us(us)


//Sets up port as output high, ready for request.
void AM2302_Init(AM2302_t* dev);

void AM2302_WaitState(AM2302_t* dev, uint8_t bitstate);
uint8_t AM2302_RequestData(AM2302_t* dev);

uint8_t AM2302_GetBit(AM2302_t* dev);

uint16_t AM2302_GetTemperature(AM2302_t* dev);
uint16_t AM2302_GetHumidity(AM2302_t* dev);



