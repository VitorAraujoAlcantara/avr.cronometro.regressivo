#include "ports_util.h"
int an_read( uint8_t pin)
{
	ADMUX = _BV(REFS0) ;
	
	switch (pin)
	{
		case PC0:
		break;
		case PC1:
		ADMUX |= _BV(MUX0);
		break;
		case PC2:
		ADMUX |= _BV(MUX1);
		break;
		case PC3:
		ADMUX |= _BV(MUX1) | _BV(MUX0);
		break;
		case PC4:
		ADMUX |= _BV(MUX2);
		break;
		case PC5:
		ADMUX |= _BV(MUX2) | _BV(MUX0);
		break;
	}
	
	ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
	
	while ( !isOn(ADCSRA, ADIF) ) _delay_ms(1);
	uint8_t low, high;
	low  = ADCL;
	high = ADCH;
	//ADCSRA = 0x00;
	return (high << 8) | low;
}