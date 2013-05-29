#include "motor.h"
#include <avr/io.h> 
#include <util/delay.h>

volatile unsigned char m1_power = 0;
volatile unsigned char m2_power = 0; // ustawiona predkosc

volatile unsigned char m1_state = 0;
volatile unsigned char m2_state = 0; // ustawiont status 0 - STOP, 1 - PRZOD, 2 - TYL

void motor_init(){

	M1_IN1_DDR |= (1 << M1_IN1);
	M1_IN2_DDR |= (1 << M1_IN2);	
	M1_PWM_DDR |= (1 << M1_PWM);
	
	M2_IN1_DDR |= (1 << M2_IN1);
	M2_IN2_DDR |= (1 << M2_IN2);	
	M2_PWM_DDR |= (1 << M2_PWM);
	
	
	TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << WGM10);
	TCCR1B |= (1 << CS11) | (1 << CS10); //inicjalizacja pwma
	
	M1 = 0;
	M2 = 0; // zatrzymujemy silniki
	
	m1_state = 0;
	m2_state = 0; //ustawiamy statusy
}


void m1_start(unsigned char dir)
{
	if(dir != m1_state)
	{
		if (dir == 1)
		{
			M1_IN1_PORT |= (1 << M1_IN1);
			M1_IN2_PORT &= ~(1 << M1_IN2);
			m1_state = 1;		
		}
		else
		{
			M1_IN1_PORT &= ~(1 << M1_IN1);
			M1_IN2_PORT |= (1 << M1_IN2);	
			m1_state = 2;	
		}
	}
	
	if(M1 != m1_power)
		M1 = m1_power; // ustawiamy pwma	
}

void m1_stop()
{
	M1 = 0;
	m1_state = 0;	
}

void m1_set(unsigned char power)
{
	m1_power = power;
	if(m1_state > 0)
		m1_start(m1_state);
}

void m1_change(char c)
{
	if(m1_power + c < 255 && m1_power + 1 > 0) 
		m1_set(m1_power + c);
}

unsigned char const m1_getspeed()
{
	return m1_power;
}


void m2_start(unsigned char dir)
{
	if(dir != m2_state)
	{
		if (dir == 1)
		{
			M2_IN1_PORT |= (1 << M2_IN1);
			M2_IN2_PORT &= ~(1 << M2_IN2);
			m2_state = 1;		
		}
		else
		{
			M2_IN1_PORT &= ~(1 << M2_IN1);
			M2_IN2_PORT |= (1 << M2_IN2);	
			m2_state = 2;	
		}
	}

	if(M2 != m2_power)
		M2 = m2_power; // ustawiamy pwma	
}

void m2_stop()
{
	M2 = 0;
	m2_state = 0;	
}

void m2_set(unsigned char power)
{
	m2_power = power;
	if(m2_state > 0)
		m2_start(m2_state);
}

void m2_change(char c)
{
	if(m2_power + c < 255 && m2_power + 1 > 0) 
		m2_set(m2_power + c);
}

unsigned char const m2_getspeed()
{
	return m2_power;
}

