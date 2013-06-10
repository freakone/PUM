#include "usart.h"
#include "motor.h"
#include "sensors.h"
#include "globals.h"
#include <avr/eeprom.h>

volatile static unsigned char buffer[30];
volatile static unsigned int pos = 0;
volatile static unsigned char hascii3[3];
volatile static unsigned char hascii8[8];
volatile static int32_t val, no;
volatile static bool bZigBee = false;

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

ISR(USART_RX_vect)
{
	if((pos > 0 && buffer[0] != 0xff )|| pos > 25) //antycrap
		pos = 0;

	buffer[pos] = UDR0;	
	pos++;
	

	if((buffer[pos-1] == 0x0A || buffer[pos-1] == 0xFE) &&  buffer[0] == 0xff && pos > 3)//sprawdzamy czy została przesłana poprawna ramka	
	{	
	
		if(buffer[1] == ADDRESS || buffer[1] == 0x10) // komendy dla wszystkich
		{
			switch(buffer[2])
			{
				case 0x70: //adres get
					
					if(bZigBee)uart_puts("AT+UCAST:0000=");
					
					uart_put(0xff);
					uart_put(buffer[1]);
					uart_put(0x70);
					dec2hascii(ADDRESS, 3);
					uart_put('\n');
					
					if(bZigBee)uart_puts("\n\r");
					break;
					
				case 0x20: //ustaw predkosc
				
				for(int i = 3; i < 11; i++)
				{					
					hascii8[i - 3] = buffer[i];
				}
				val = hascii2dec(hascii8, 8);
				
				if(val > 0)
				{
					m1_set(val);
					m2_set(val);
					m1_start(1);
					m2_start(1);
				}
				else
				{
					m1_set(-1*val);
					m2_set(-1*val);
					m1_start(2);
					m2_start(2);				
				}
				break;
				
				case 0x22: //przyrost predkosci
				
				for(int i = 3; i < 11; i++)
				{					
					hascii8[i - 3] = buffer[i];
				}
				val = hascii2dec(hascii8, 8);
				
				m1_change(val);
				m2_change(val);
				break;
				
				case 0x24: //zmniejszenie predkosci
				
				for(int i = 3; i < 11; i++)
				{					
					hascii8[i - 3] = buffer[i];
				}
				val = hascii2dec(hascii8, 8);
				
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
				
				ADDRESS = hascii2dec(hascii3, 3);
				eeprom_write_word((uint16_t*)1, (uint16_t)ADDRESS);
				
				if(bZigBee)uart_puts("AT+UCAST:0000=");
				uart_put(0xff);
				uart_put(ADDRESS);
				uart_put(0x70);
				dec2hascii(ADDRESS, 3);
				uart_put('\n');
				if(bZigBee)uart_puts("\n\r");
				break;
				
				case 0x26: //regulacja prędkości pojedynczego silnika
				
				
				for(int i = 3; i < 6; i++)
				{					
					hascii3[i - 3] = buffer[i];
				}

				no = hascii2dec(hascii3, 3);
				
				for(int i = 6; i < 14; i++)
				{					
					hascii8[i - 6] = buffer[i];

				}
				val = hascii2dec(hascii8, 8);
				
					switch(no)
					{
						case 0:
							if(val > 0)
							{
								m1_set(val);								
								m1_start(1);
							}
							else
							{								
								m1_set(-1*val);
								m1_start(2);				
							}
							break;
						case 1:
							if(val > 0)
							{
								m2_set(val);								
								m2_start(1);
							}
							else
							{								
								m2_set(-1*val);
								m2_start(2);				
							}
							break;						
					}
					
				break;
				
				case 0x61: // odczyt adc
				if(bZigBee)uart_puts("AT+UCAST:0000=");
				
				sensors_update();				
				uart_put(0xff);
				uart_put(ADDRESS);
				uart_put(0x61);
				for(int i = 0; i < SENS; i++)
					dec2hascii(sensor[i], 3);
				uart_put('\n');
				if(bZigBee)uart_puts("\n\r");
				break;
				
				case 0x64: // odczyta predkosci				
				
				if(bZigBee)uart_puts("AT+UCAST:0000=");
				uart_put(0xff);
				uart_put(ADDRESS);
				uart_put(0x64);		
				if(m1_getdir() < 2)
					uart_puts("0000");
				else
					uart_puts("FFFF");
				dec2hascii(m1_getspeed(), 4);
				
				if(m2_getdir() < 2)
					uart_puts("0000");
				else
					uart_puts("FFFF");
				dec2hascii(m2_getspeed(), 4);
				uart_put('\n');
				if(bZigBee)uart_puts("\n\r");
				break;
				
				
				case 0x90: //oczyt funkcji		
				if(bZigBee)uart_puts("AT+UCAST:0000=");
				uart_put(0xff);
				uart_put(ADDRESS);
				uart_put(0x90);				
				dec2hascii(3, 3);
				uart_put('\n');
				if(bZigBee)uart_puts("\n\r");
				break;
				
				case 0x91: //zigbee on
				bZigBee = true;
				uart_puts("AT+UCAST:0000=");
				uart_put(0xff);
				uart_put(ADDRESS);
				uart_put(0x91);
				uart_put('\n');		
				uart_puts("\n\r");				
				break;
				
				
				case 0x92: // zigbee off
				bZigBee = false;
				uart_put(0xff);
				uart_put(ADDRESS);
				uart_put(0x92);
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
