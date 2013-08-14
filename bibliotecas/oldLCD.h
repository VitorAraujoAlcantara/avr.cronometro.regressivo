//#define DADOS_LCD	PORTD
#define DADOS_LCD	0b0000000
#define RS			PD7
#define E			PB0
unsigned char i=1;

#define LCD_linha1 LCD_cmd(0x80,0)
#define LCD_linha2 LCD_cmd(0xC0,0)
#define LCD_clear LCD_cmd(0x01,0); _delay_ms(2)

uint8_t _data_pins[4];


void send(uint8_t value, uint8_t mode) {
	if ( mode == 0 )
	{
		clr_bit(PORTD, RS);		
	}
	else
	{
		set_bit(PORTD,RS);
		
	}
	
	write4bits(value>>4);
	write4bits(value);
	
}

void  write4bits(uint8_t value) {
  for (int i =  0; i < 4; i++) {
	  //set_bit(DDRD, _data_pins[i]);
	  if ((value >> i) & 0x01)
	  {
		  set_bit(PORTD,_data_pins[i]);		  
	  }
	  else
	  {
		  clr_bit(PORTD,_data_pins[i]);		  
	  }	  	  
  }
  pulseEnable();
}

void pulseEnable(void) {
	clr_bit( PORTB, E);
	_delay_us(1);
	set_bit(PORTB, E);
	_delay_us(1);
	clr_bit(PORTB,E);
	_delay_us(100);
}

//------------------------------------------
//Sub-rotina para enviar comando ao LCD
void LCD_cmd (unsigned char c, char cd)
{
	
	set_bit(DDRD,RS);	
	set_bit(DDRB,E);
	
	_data_pins[0] = PORTD6;
	_data_pins[1] = PORTD5;
	_data_pins[2] = PORTD4;
	_data_pins[3] = PORTD3;
	for(int i = 0; i < 4; i++){
		set_bit(DDRD, _data_pins[i]);		
	}
	
	send(c, cd);
}


/*void LCD_cmd (unsigned char c, char cd)
{
	unsigned char _DADOS_LCD = c;
	
	if ( tst_bit(_DADOS_LCD, 3 ) )
	{
		set_bit( PORTD, D7);
	}
	else
	{
		clr_bit( PORTD, D7);
	}
	
	if ( tst_bit(_DADOS_LCD, 2 ) )
	{
		set_bit( PORTD, D6);
	}
	else
	{
		clr_bit( PORTD, D6);
	}
	
	if ( tst_bit(_DADOS_LCD, 1 ))
	{
		set_bit( PORTD, D5);
	}
	else
	{
		clr_bit( PORTD, D5);
	}
	
	if ( tst_bit(_DADOS_LCD, 0 ))
	{
		set_bit( PORTD, D4);
	}
	else
	{
		clr_bit( PORTD, D4);
	}
	
	for (;i!=0;i--)
	{		
		set_bit(DDRD,RS);
		set_bit(DDRD,D7);
		set_bit(DDRD,D6);
		set_bit(DDRD,D5);
		set_bit(DDRD,D4);
		set_bit(DDRB,E);
		
		if (cd==0)
		clr_bit(PORTD,RS); // RS = 0
		else
		set_bit(PORTD,RS); // RS = 1
		
		set_bit(PORTB,E);		// E = 1
		clr_bit(PORTB,E);		// E = 0
		
		_delay_us(45);
		
		if ((cd==0) && (c<127)) // se for instrução espera tempo de resposta do display
		_delay_us(2);
		_DADOS_LCD = c<<4;		// 4 bits menos significativos (nibble)
				
		
		
		if ( tst_bit(_DADOS_LCD, 3 ) )
		{			
			set_bit( PORTD, D7);
		}
		else
		{
			clr_bit( PORTD, D7);
		}
			
		if ( tst_bit(_DADOS_LCD, 2 ) )
		{
			set_bit( PORTD, D6);
		}
		else
		{
			clr_bit( PORTD, D6);
		}
		
		if ( tst_bit(_DADOS_LCD, 1 ))
		{
			set_bit( PORTD, D5);
		}
		else
		{
			clr_bit( PORTD, D5);
		}
			
		if ( tst_bit(_DADOS_LCD, 0 ))
		{
			set_bit( PORTD, D4);
		}
		else
		{
			clr_bit( PORTD, D4);
		}
		
	}
	i = 2;
}*/

//-----------------------------------------------
//Sub-rotina de inicialização do LCD
//------------------------------------------------
void LCD_inicia(void)	// envio de instruções para o LCD
{
	LCD_cmd(0x28,0);   //interface de 4 bits
	_delay_us(39);
	LCD_cmd(0x28,0);   //interface de 4 bits 2 linhas (aqui serao habilitadas as 2 linhas)
	_delay_us(39);
	LCD_cmd(0x0C,0);  //mensagem aparente cursor ativo piscando
	_delay_us(39);
	LCD_cmd(0x01,0);  //limpa todo o display
	_delay_ms(1.53);
	LCD_cmd(0x80,0);   //escreve na primeira posição a esquerda – 1ª linha

}

//-------------------------------------------------
//Sub-rotina de escrita do LCD
//-------------------------------------------------
void LCD_escreve(char *c)
{
	for(; *c!=0;c++)
	{
		LCD_cmd(*c,1)	;
		//_delay_ms(1);
	}
}
