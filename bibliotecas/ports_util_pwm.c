#include "ports_util.h"
static uint8_t ocr0a = 0;
static uint8_t ocr0b = 0;
static uint8_t ocr1a = 0;
static uint8_t ocr1b = 0;
static uint8_t ocr2a = 0;
static uint8_t ocr2b = 0;


void pwm_OCR0_disable()
{
	TCCR0A = 0x00;
	TCCR0B = 0x00;
	
}
void pwm_OCR1_disable()
{	
	TCCR1A = 0x00;
	TCCR1B = 0x00;	
}
void pwm_OCR2_disable()
{
	TCCR2A = 0x00;
	TCCR2B = 0x00;	
}

void pwm_OCR1_init (uint8_t _OCR1A, uint8_t _OCR1B) // PINOS 15 E 16
{
	ocr1a = _OCR1A;
	ocr1b = _OCR1B;
	if ( ocr1a )
	{
		on(DDRB,PB1);
		off(PORTB,PB1);
	}
	if ( ocr2b )
	{
		on(DDRB,PB2);	
		off(PORTB,PB2)	;
	}
	OCR1A = 0;
	OCR1B = 0;	

	TCCR1A |= (_OCR1A << COM1A1)|(_OCR1B << COM1B1);	
	TCCR1A |= (1 << WGM10);
	TCCR1B |= (1 << WGM12);	
	TCCR1B |= _BV(CS12) ;
}
void pwm_OCR0_init (  uint8_t  _OCR0A, uint8_t _OCR0B )  // PINOS 12 E 11
{
		OCR0A = 0;
		OCR0B = 0;
		
		ocr0a = _OCR0A;
		ocr0b = _OCR0B;
		
		if ( ocr0a )
		{
			on(DDRD,PD6);
			off(PORTD,PD6);
		}
		
		if ( ocr0b )
		{
			on(DDRD,PD5);
			off(PORTD,PD5);
		}
		
		TCCR0A =     _OCR0A <<  COM0A1 |    /* not inverted */
		0 << COM0A0 |
		_OCR0B << COM0B1 |    /* not inverted */
		0 << COM0B0 |
		1 << WGM01  |    /* fast pwm */
		1 << WGM00  ;    /* fast pwm */

		TCCR0B = 0 << FOC0A |
		0 << FOC0B |
		0 << WGM02 |     /* fast pwm */
		1 << CS02  |     /* div 256 prescale */
		0 << CS01  |
		0 << CS00  ;
}
void pwm_OCR2_init ( uint8_t _OCR2A, uint8_t _OCR2B)//PINOS 5 E 17
{
		ocr2a = _OCR2A;
		ocr2b = _OCR2B;
		if (ocr2a)
		{
			on(DDRB,PB3);
			off(PORTB,PB3);
		}
		
		if (ocr2b)
		{
			on(DDRD,PD3);
			off(PORTD,PD3);
		}
		OCR2A = 0;	
		OCR2B = 0;
		TCCR2A |= (_OCR2A << COM2A1)|(_OCR2B << COM2B1);
		TCCR2A |= (1 << WGM20);
		TCCR2A |= (1 << WGM21);
		TCCR2B |= _BV(CS22) | _BV(CS21) ;
}

void pwm_OCR0( uint8_t _OCR0A, uint8_t _OCR0B )
{		
	if (ocr0a )
	OCR0A = _OCR0A;
	if (ocr0b)
	OCR0B = _OCR0B;
}
void pwm_OCR1( uint8_t _OCR1A, uint8_t _OCR1B)
{
	if (ocr1a)
		OCR1A = _OCR1A;
	if (ocr1b)
		OCR1B = _OCR1B;	
}
void pwm_OCR2( uint8_t _OCR2A, uint8_t _OCR2B)
{
	if (ocr2a)
	OCR2A = _OCR2A;
	if (ocr2b)
	OCR2B = _OCR2B;	
}