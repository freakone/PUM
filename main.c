#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include "libs/usart.h"
#include "libs/sensors.h"
#include "libs/motor.h"
#include "libs/globals.h"
#include <avr/eeprom.h>



int main()
{
	ADDRESS = eeprom_read_word(( uint16_t *)1);	
	if(ADDRESS == 0xff) // temporary
		ADDRESS = 0x0C;
		
	sei();	
	uart_init();
	motor_init();
	sensors_init();
	
	while(1)
	{		

	}
}