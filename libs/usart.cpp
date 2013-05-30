#include "usart.h"
#include "motor.h"
#include "sensors.h"
#include "globals.h"
#include <avr/eeprom.h>

static unsigned char buffer[30];
static unsigned int pos = 0;
void uart_init()
{	
	UBRR0H = (unsigned char)(BAUD_PRESCALE>>8);
	UBRR0L = (unsigned char)BAUD_PRESCALE;
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
	UCSR0C = (3<<UCSZ00);	
	pos = 0;
}

unsigned char uart_receive( void )
{
	while ( !(UCSR0A & (1<<RXC0)) ) 	
		;			                

	return UDR0;
}

volatile int8_t hascii3[3];
volatile int8_t hascii8[8];
volatile int32_t val, no;
ISR(USART_RX_vect)
{
	if((pos > 0 && buffer[0] != 0xff )|| pos > 25) //antycrap
		pos = 0;

	buffer[pos] = UDR0;	
	pos++;
	

	if(buffer[pos-1] == 0x0A &&  buffer[0] == 0xff)//sprawdzamy czy została przesłana poprawna ramka	
	{	
	
		if(buffer[1] == ADDRESS || buffer[1] == 0x10) // komendy dla wszystkich
		{
			switch(buffer[2])
			{
				case 0x70: //adres get
					
					uart_put(0xff);
					uart_put(buffer[1]);
					uart_put(0x70);
					dec2hascii(ADDRESS, 3);
					uart_put('\n');
					break;
					
				case 0x20: //ustaw predkosc
				
				for(int i = 3; i < 11; i++)
				{					
					hascii8[i - 3] = buffer[i];
				}
				val = hascii2dec(hascii8);
				
				m1_set(val);
				m2_set(val);
				break;
				
				case 0x22: //przyrost predkosci
				
				for(int i = 3; i < 11; i++)
				{					
					hascii8[i - 3] = buffer[i];
				}
				val = hascii2dec(hascii8);
				
				m1_change(val);
				m2_change(val);
				break;
				
				case 0x24: //zmniejszenie predkosci
				
				for(int i = 3; i < 11; i++)
				{					
					hascii8[i - 3] = buffer[i];
				}
				val = hascii2dec(hascii8);
				
				m1_change(-val);
				m2_change(-val);
				break;
			}
		
		}

		if(buffer[1] == ADDRESS) // komendy jednostkowe
		{
		
			switch(buffer[2])
			{			
				
				case 0x71: // adres set
				for(int i = 3; i < 6; i++)
				{					
					hascii3[i - 3] = buffer[i];
				}
				
				ADDRESS = hascii2dec(hascii3);
				eeprom_write_word((uint16_t*)1, (uint16_t)ADDRESS);
				
				uart_put(0xff);
				uart_put(ADDRESS);
				uart_put(0x70);
				dec2hascii(ADDRESS, 3);
				uart_put('\n');
				break;
				
				case 0x26: //regulacja prędkości pojedynczego silnika
				for(int i = 3; i < 6; i++)
				{					
					hascii3[i - 3] = buffer[i];
				}
				no = hascii2dec(hascii3);
				
				for(int i = 6; i < 14; i++)
				{					
					hascii8[i - 6] = buffer[i];
				}
				val = hascii2dec(hascii8);
				
					switch(no)
					{
						case 0:
							m1_set(val);
							m1_start(0);
							break;
						case 1:
							m1_set(val);
							m1_start(1);
							break;
						case 2:
							m2_set(val);
							m2_start(0);
							break;
						case 3:
							m2_set(val);
							m2_start(1);
							break;					
					}
					
				break;
				
				case 0x61: // odczyt adc
				
				sensors_update();
				
				uart_put(0xff);
				uart_put(ADDRESS);
				uart_put(0x61);
				for(int i = 0; i < SENS; i++)
					dec2hascii(sensor[i], 3);
				uart_put('\n');
				break;
				
				case 0x64: // odczyta predkosci				
				
				uart_put(0xff);
				uart_put(ADDRESS);
				uart_put(0x64);				
				dec2hascii(m1_getspeed(), 8);
				dec2hascii(m2_getspeed(), 8);
				uart_put('\n');
				break;
				
				
				case 0x90: //oczyt funkcji		
				
				uart_put(0xff);
				uart_put(ADDRESS);
				uart_put(0x90);				
				dec2hascii(3, 3);
				uart_put('\n');
				break;
			}
			
		}	
		
			
		pos = 0;
	
	}

	
}

void uart_put( unsigned char data )
{
	while(!( UCSR0A & (1<<UDRE0)));
	UDR0 = data;		        
}

void uart_puts(const char *s )
{
    while (*s)
      uart_put(*s++);
}

void uart_puts(unsigned char *s )
{
    while (*s)
      uart_put(*s++);
}
