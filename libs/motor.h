#define M1_IN1_PORT PORTB //SILNIK LEWY
#define M1_IN1_DDR DDRB
#define M1_IN1 PB0

#define M1_IN2_PORT PORTD
#define M1_IN2_DDR DDRD
#define M1_IN2 PD7

#define M1_PWM_DDR DDRB
#define M1 OCR1B
#define M1_PWM PB2 


#define M2_IN1_PORT PORTD //SILNIK PRAWY
#define M2_IN1_DDR DDRD
#define M2_IN1 PD5

#define M2_IN2_PORT PORTD
#define M2_IN2_DDR DDRD
#define M2_IN2 PD6

#define M2_PWM_DDR DDRB
#define M2 OCR1A
#define M2_PWM PB1

#define FOR 1
#define BCK 2


void motor_init();
void m1_start(unsigned char dir);
void m1_stop();
void m1_set(unsigned char power);
void m1_change(char c);
unsigned char const m1_getspeed();
void m2_start(unsigned char dir);
void m2_stop();
void m2_set(unsigned char power);
void m2_change(char c);
unsigned char const m2_getspeed();
