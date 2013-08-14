/*
 * temporizador.c;
 *
 *  Created on: 09/08/2013
 *      Author: vitor
 */
#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "lcd.c"
#include <stdlib.h>
#include <avr/interrupt.h>
#include "ports_util.h"

#define ESPERA_BT 30
#define ST_NORMAL 0
#define ST_CRONOMETRO_REGRESSIVO 1

#define PORT_LED PORTB
#define DDR_LED DDRB
#define PINO_LED PB2

#define PORT_RELE PORTB
#define DDR_RELE DDRB
#define PINO_RELE PB0

#define PORT_BT1 PORTB
#define DDR_BT1 DDRB
#define PIN_BT1 PINB
#define PINO_BT1  PB5

#define PORT_BT2 PORTB
#define DDR_BT2 DDRB
#define PIN_BT2 PINB
#define PINO_BT2  PB4

#define PORT_BT3 PORTB
#define DDR_BT3 DDRB
#define PIN_BT3 PINB
#define PINO_BT3  PB3

#define PORT_CANCELA PORTC
#define DDR_CANCELA DDRC
#define PIN_CANCELA PINC
#define PINO_CANCELA  PC4

#define PORT_PROG PORTC
#define DDR_PROG DDRC
#define PIN_PROG PINC
#define PINO_PROG  PC3

#define PORT_MAIS PORTC
#define DDR_MAIS DDRC
#define PIN_MAIS PINC
#define PINO_MAIS  PC2

#define PORT_MENOS PORTC
#define DDR_MENOS DDRC
#define PIN_MENOS PINC
#define PINO_MENOS  PC1

#define PORT_BUZINA PORTC
#define DDR_BUZINA DDRC
#define PIN_BUZINA PINC
#define PINO_BUZINA PC0

static uint8_t enderecos[3][5] = { { 1, 2, 3, 4, 5 }, { 6, 7, 8, 9, 10 }, { 11,
		12, 13, 14, 15 } };

static uint8_t status;

static int horas = 0;
static int minutos = 0;
static int segundos = 0;
static int milisegundos = 0;

void escrevePadrao() {
	lcd_gotoxy(0, 0);
	lcd_puts("  TEMPORIZADOR  ");
	lcd_gotoxy(0, 1);
	lcd_puts("<<            >>");
}

void ativaRele() {
	on(DDR_RELE, PINO_RELE);
	on(PORT_RELE, PINO_RELE);
}

void desativaRele() {
	off(PORT_RELE, PINO_RELE);
}

void ativaCronometroRegressivo(uint8_t hora, uint8_t minuto, uint8_t segundo,
		uint8_t milisegundo) {
	on(DDR_LED, PINO_LED);
	on(PORT_LED, PINO_LED);
	horas = hora;
	minutos = minuto;
	segundos = segundo;
	milisegundos = milisegundo;

	cli();

	TCCR1B |= (1 << WGM12); // Configure timer 1 for CTC mode

	TIMSK1 |= (1 << OCIE1A); // Enable CTC interrupt

	sei();
	//  Enable global interrupts

	OCR1A = 250; // Set CTC compare value to 1Hz at 1MHz AVR clock, with a prescaler of 64

	TCCR1B |= ((1 << CS10) | (1 << CS11)); // Start timer at Fcpu/64
	ativaRele();
	uint8_t b = 0;

	while (((horas > 0) || (minutos > 0) || (segundos > 0) || (milisegundos > 0))
			&& (b == 0)) {
		if (!isOn(PIN_CANCELA,PINO_CANCELA)) {
			if (!isOn(PIN_CANCELA,PINO_CANCELA)) {
				status = ST_NORMAL;
				_delay_ms(ESPERA_BT);
				desativaRele();
				lcd_gotoxy(0, 0);
				lcd_puts("   CANCELADO    ");
				_delay_ms(1000);
				b = 1;
				off(PORT_LED, PINO_LED);

				break;
			}
		}
		if (b == 0) {
			lcd_gotoxy(0, 1);
			char tempo[12];
			sprintf(tempo, "  %02d:%02d:%02d.%03d  ", horas, minutos, segundos,
					milisegundos);
			lcd_puts(tempo);

		}
		_delay_ms(10);
	}
	desativaRele();
	off(PORT_LED, PINO_LED);

	lcd_gotoxy(0, 0);
	lcd_puts("<<<< TERMINO >>>>");
	lcd_gotoxy(0, 1);
	char tempo[12];
	sprintf(tempo, "  %02d:%02d:%02d.%03d  ", horas, minutos, segundos, 0);
	lcd_puts(tempo);
	on(DDR_BUZINA, PINO_BUZINA);
	for (int i = 0; i < 3; i++) {
		on(PORT_BUZINA, PINO_BUZINA);
		_delay_ms(150);
		off(PORT_BUZINA, PINO_BUZINA);
		_delay_ms(150);
	}

	status = ST_NORMAL;
	cli();

}

void executaProgramacao(unsigned char numero) {
	uint8_t hora;
	uint8_t minuto;
	uint8_t segundo;
	uint16_t milisegundo;

	hora = eeprom_read_byte(enderecos[numero - 1][0]);
	minuto = eeprom_read_byte(enderecos[numero - 1][1]);
	segundo = eeprom_read_byte(enderecos[numero - 1][2]);
	milisegundo = eeprom_read_byte(enderecos[numero - 1][3]) << 8
			| eeprom_read_byte(enderecos[numero - 1][4]);

	hora = hora < 100 ? hora : 0;
	minuto = minuto < 60 ? minuto : 0;
	segundo = segundo < 60 ? segundo : 0;
	milisegundo = milisegundo < 1000 ? milisegundo : 0;

	ativaCronometroRegressivo(hora, minuto, segundo, milisegundo);
	escrevePadrao();
}

unsigned char setProgramcao(uint8_t numero) {

	uint8_t hora;
	uint8_t minuto;
	uint8_t segundo;
	uint16_t milisegundo;

	hora = eeprom_read_byte(enderecos[numero - 1][0]);
	minuto = eeprom_read_byte(enderecos[numero - 1][1]);
	segundo = eeprom_read_byte(enderecos[numero - 1][2]);
	milisegundo = eeprom_read_byte(enderecos[numero - 1][3]) << 8
			| eeprom_read_byte(enderecos[numero - 1][4]);

	hora = hora < 100 ? hora : 0;
	minuto = minuto < 60 ? minuto : 0;
	segundo = segundo < 60 ? segundo : 0;
	milisegundo = milisegundo < 1000 ? milisegundo : 0;

	char tempo[16];
	lcd_gotoxy(0, 1);
	sprintf(tempo, "  %02d:%02d:%02d.%03d  ", hora, minuto, segundo,
			milisegundo);
	lcd_puts(tempo);
	lcd_gotoxy(2, 1);

	unsigned char pos = 1;
	unsigned char b = 0;
	while (b == 0) {
		unsigned char op = ' ';
		if (!isOn(PIN_CANCELA, PINO_CANCELA)) {
			_delay_ms(ESPERA_BT);
			_delay_ms(ESPERA_BT);
			if (!isOn(PIN_CANCELA, PINO_CANCELA)) {
				b = 1;
			}
		} else

		if (!isOn(PIN_MAIS, PINO_MAIS)) {
			_delay_ms(ESPERA_BT);
			_delay_ms(ESPERA_BT);
			if (!isOn(PIN_MAIS, PINO_MAIS)) {
				op = '+';
			}
		} else if (!isOn(PIN_MENOS, PINO_MENOS)) {
			_delay_ms(ESPERA_BT);
			_delay_ms(ESPERA_BT);
			if (!isOn(PIN_MENOS, PINO_MENOS)) {
				op = '-';
			}
		} else if (!isOn(PIN_PROG, PINO_PROG)) {
			_delay_ms(ESPERA_BT);
			_delay_ms(ESPERA_BT);
			if (!isOn(PIN_PROG, PINO_PROG)) {
				if (pos > 3) {
					op = 'w';

				} else {
					pos++;

					switch (pos) {
					case 1:
						lcd_gotoxy(2, 1);
						break;
					case 2:
						lcd_gotoxy(5, 1);
						break;
					case 3:
						lcd_gotoxy(8, 1);
						break;
					default:
						lcd_gotoxy(11, 1);
						break;
					}

					_delay_ms(ESPERA_BT);
				}
			}
		}

		if ((op == '-') || (op == '+')) {
			switch (pos) {
			case 1:
				if (op == '-') {
					if (hora > 0) {
						hora--;
					} else {
						hora = 99;
					}
				} else if (hora < 99) {
					hora++;
				} else {
					hora = 0;
				}

				break;
			case 2:
				if (op == '-') {
					if (minuto > 0) {
						minuto--;
					} else {
						minuto = 59;
					}
				} else if (minuto < 59) {
					minuto++;
				} else {
					minuto = 0;
				}
				break;
			case 3:
				if (op == '-') {
					if (segundo > 0) {
						segundo--;
					} else {
						segundo = 59;
					}
				} else if (segundo < 59) {
					segundo++;
				} else {
					segundo = 0;
				}
				break;
			case 4:
				if (op == '-') {
					if (milisegundo > 0) {
						milisegundo--;
					} else {
						milisegundo = 999;
					}
				} else {
					if (milisegundo < 999) {
						milisegundo++;
					} else {
						milisegundo = 0;
					}
				}
				break;
			}

			char tempo[16];
			lcd_gotoxy(0, 1);
			sprintf(tempo, "  %02d:%02d:%02d.%03d  ", hora, minuto, segundo,
					milisegundo);
			lcd_puts(tempo);
			switch (pos) {
			case 1:
				lcd_gotoxy(2, 1);
				break;
			case 2:
				lcd_gotoxy(5, 1);
				break;
			case 3:
				lcd_gotoxy(8, 1);
				break;
			default:
				lcd_gotoxy(11, 1);
				break;
			}

		} else if (op == 'w') {
			eeprom_write_byte(enderecos[numero - 1][0], hora);
			eeprom_write_byte(enderecos[numero - 1][1], minuto);
			eeprom_write_byte(enderecos[numero - 1][2], segundo);
			eeprom_write_byte(enderecos[numero - 1][3], milisegundo >> 8);
			eeprom_write_byte(enderecos[numero - 1][4],
					(milisegundo << 8) >> 8);
			b = 1;
			lcd_gotoxy(0, 1);
			char c[16];
			sprintf(c, "GRAVADO: %d     ", numero);
			lcd_puts(c);
			_delay_ms(2000);
		}

		_delay_ms(50);

	}

	return 1;
}

void getComando() {
	if (!isOn(PIN_BT1, PINO_BT1)) {
		_delay_ms(ESPERA_BT);
		if (!isOn(PIN_BT1, PINO_BT1)) {
			executaProgramacao(1);
		}
	} else if (!isOn(PIN_BT2, PINO_BT2)) {
		_delay_ms(ESPERA_BT);
		if (!isOn(PIN_BT2, PINO_BT2)) {
			executaProgramacao(2);
		}
	} else if (!isOn(PIN_BT3, PINO_BT3)) {
		_delay_ms(ESPERA_BT);
		if (!isOn(PIN_BT3, PINO_BT3)) {
			executaProgramacao(3);
		}
	} else if (!isOn(PIN_PROG, PINO_PROG)) {
		_delay_ms(ESPERA_BT);
		if (!isOn(PIN_PROG, PINO_PROG)) {
			unsigned char b = 0;
			unsigned char n = 0;
			lcd_gotoxy(0, 0);
			lcd_puts("SELECIONE PROGR.");
			lcd_gotoxy(0, 1);
			lcd_puts("PRESSIONE N.:   ");

			while (b == 0) {
				if (!isOn(PIN_CANCELA, PINO_CANCELA )) {
					_delay_ms(ESPERA_BT);
					if (!isOn(PIN_CANCELA, PINO_CANCELA )) {
						b = 1;
						lcd_gotoxy(0, 0);
						lcd_puts("CANCELADO PELO  ");
						lcd_gotoxy(0, 1);
						lcd_puts("    OPERADOR    ");
						_delay_ms(2000);
					}
				} else if (!isOn(PIN_MENOS, PINO_MENOS)) {
					_delay_ms(ESPERA_BT);
					if (!isOn(PIN_MENOS, PINO_MENOS)) {
						if (n < 2) {
							n = 3;
						} else {
							n--;
						}
					}
				} else if (!isOn(PIN_MAIS, PINO_MAIS)) {
					_delay_ms(ESPERA_BT);
					if (!isOn(PIN_MAIS, PINO_MAIS)) {
						if (n > 2) {
							n = 1;
						} else {
							n++;
						}
					}
				} else if (!isOn(PIN_BT1, PINO_BT1)) {
					_delay_ms(ESPERA_BT);
					if (!isOn(PIN_BT1, PINO_BT1)) {
						n = 1;
					}
				} else if (!isOn(PIN_BT2, PINO_BT2)) {
					_delay_ms(ESPERA_BT);
					if (!isOn(PIN_BT2, PINO_BT2)) {
						n = 2;
					}
				} else if (!isOn(PIN_BT3, PINO_BT3)) {
					_delay_ms(ESPERA_BT);
					if (!isOn(PIN_BT3, PINO_BT3)) {
						n = 3;
					}
				} else {
					if ((n > 0) && (!isOn(PIN_PROG, PINO_PROG))) {
						_delay_ms(ESPERA_BT);
						if (!isOn(PIN_PROG, PINO_PROG)) {

							lcd_gotoxy(0, 0);
							lcd_puts("INICIANDO PROGR  ");
							lcd_gotoxy(0, 1);
							lcd_puts("                 ");
							_delay_ms(2000);
							b = setProgramcao(n);

						}
					}
				}

				if (b == 0) {
					lcd_gotoxy(14, 1);
					char ch[1];
					sprintf(ch, "%d", n);
					lcd_puts(ch);
				}

				_delay_ms(50);

			}

		}
	}

	escrevePadrao();

}

int main(void) {
	lcd_init(LCD_DISP_ON_CURSOR_BLINK);
	escrevePadrao();

	on(DDR_LED, PINO_LED);
	off(PORT_LED, PINO_LED);

	off(DDR_BT1, PINO_BT1);
	on(PORT_BT1, PINO_BT1);

	off(DDR_BT2, PINO_BT2);
	on(PORT_BT2, PINO_BT2);

	off(DDR_BT3, PINO_BT3);
	on(PORT_BT3, PINO_BT3);

	off(DDR_CANCELA, PINO_CANCELA);
	on(PORT_CANCELA, PINO_CANCELA);  // CANCEL

	off(DDR_PROG, PINO_PROG); // PROG
	on(PORT_PROG, PINO_PROG);

	off(DDR_MAIS, PINO_MAIS); // PLUS
	on(PORT_MAIS, PINO_MAIS);

	off(DDR_MENOS, PINO_MENOS); // MINUS
	on(PORT_MENOS, PINO_MENOS);

	on(DDR_BUZINA, PINO_BUZINA);
	off(PORT_BUZINA, PINO_BUZINA);

	status = ST_NORMAL;

	while (1 == 1) {
		getComando();
		_delay_ms(50);
	}

	return 0;

}

ISR(TIMER1_COMPA_vect) {
	if ((horas <= 0) && (minutos <= 0) && (segundos <= 0)
			&& (milisegundos <= 0)) {
		cli();
	} else {
		if (milisegundos > 0) {
			milisegundos--;
		} else {
			if (segundos > 0) {
				milisegundos = 999;
				segundos--;
			} else {
				if (minutos > 0) {
					minutos--;
					segundos = 59;
					milisegundos = 999;
				} else {
					if (horas > 0) {
						horas--;
						minutos = 59;
						segundos = 59;
						milisegundos = 999;
					} else {
						cli();

					}
				}
			}

		}

	}

}

