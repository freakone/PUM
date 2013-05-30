#include "globals.h"
#include "usart.h"

volatile unsigned char ADDRESS;

void wait_ms(int ms) {
  int i;
  for (i=0; i<ms; i++) {
    _delay_ms(1);
  }
}

void dec2hascii(uint32_t liczba, uint8_t length){
	
	uint32_t buffer;
	
	for (int i=length-1;i >= 0;i--)
	{
		buffer = (liczba % (1<< ((i+1)*4))) / (1<< ((i*4))) ;
		if (buffer>=0 && buffer<=9)
			uart_put((uint8_t)buffer+48);
		else if (buffer>=10 && buffer<=15)
			uart_put((uint8_t)buffer+55);
		
	}
}

int32_t hascii2dec(volatile int8_t* ptext){
	volatile int8_t* p;
	int32_t t = 0;
	int l = 0;

	p=ptext;
	
	while(*p)
	{		
		l++;
		*p++;
	}
	
	
	p=ptext;	

	for(int i = l-1; i >= 0; i--)
	{
		
		
		if(*p>='0' && *p<='9'){
		t+=(*p-48)*(1<< (i*4));
		}else if(*p>='A' && *p<='F'){
			t+=(*p-55)*(1<< (i*4));
		}
		++p;	
	}

	return t;

}