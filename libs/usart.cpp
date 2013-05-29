#include "usart.h"
#include "motor.h"
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
ISR(USART_RX_vect)
{
	buffer[pos] = UDR0;	
	pos++;
	

	if(UDR0 == 0x0A &&  buffer[0] == 0xff)//sprawdzamy czy została przesłąna poprawna ramka	
	{	
		if(buffer[1] == ADDRESS || buffer[1] == 0x10) // komendy jednostkowe
		{
		
			switch(buffer[2])
			{
				case 0x70:
				
				uart_put(0xff);
				uart_put(buffer[1]);
				uart_put(0x70);
				dec2hascii(ADDRESS, 3);
				uart_put('\n');
				break;
				
				case 0x71:
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
			
			}
			
		}
		else if(buffer[1] == 0x10) // komendy dla wszystkich
		{
		
		
		}
		
		
			
		pos = 0;
	
	}
	else if((pos > 1 && buffer[0] != 0xff )|| pos > 25)
		pos = 0;

	
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
