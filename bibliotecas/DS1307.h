#include <stdbool.h> 
#include <compat/twi.h>		// BIBLIOTECA WIRE
#define DS_SLA 0b11010000 
#define DS_CH 7 
#define DS_24H 6 

static char DS1307_Time[9];
static uint8_t vlr;

/************************************************ 
   DS1307 FUNCTIONS 
*************************************************/ 

void TWI_Init() 
{ 
   /* 
   Initiate TWI interface and select 
   SCL Clock frequency according to: 
   SCLf=FCPU/(16+2*TWBR*(4^TWPS)) 
   For ATmega8A max freq is 400kHZ 
   When choosing SCL freq remember 
   that devices attached via TWI interface 
   may have limitation (example DS1307 can operate 
   on max 100kHZ SCL frequency) 
    
   TWPS1|TWPS0|TWPS(prescaler val) 
     0  |  0  |       1 
     0  |  1  |       4 
     1  |  0  |      16 
     1  |  1  |      64 
   */ 
   TWBR=0b00001000; //TWBR->8 
   TWSR&=~(1<<TWPS1); 
   TWSR&=~(1<<TWPS0); 
   // With FCPU 4MHZ SCL freq is 50kHZ 
   TWCR|=(1<<TWEN); 
} 

void TWI_Deactivate() 
{ 
   TWCR&=~(1<<TWEN); 
} 

void TWI_Start() 
{ 
   // Send start condition and wait for finish 
   TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN); 
   while(!(TWCR&(1<<TWINT))); 
} 

void TWI_Stop() 
{ 
   //Send stop condition 
   TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO); 
} 

bool TWI_WriteByte(uint8_t byte) 
{ 
   TWDR=byte; 
   //Start data transfer and wait for finish 
   TWCR=(1<<TWINT)|(1<<TWEN); 
   while(!(TWCR&(1<<TWINT))); 
   /* 
   Check if completed successfully one of statuses: SLA+W transmitted 
   and ACK received, SLA+R transmitted and ACK received, DATA transmitted 
   and ACK received 
   */ 
   if ((TW_STATUS==TW_MT_SLA_ACK)||(TW_STATUS==TW_MT_DATA_ACK)||(TW_STATUS==TW_MR_SLA_ACK)) 
   { 
      return true; 
   } 
   else 
   { 
      return false; 
   } 
} 

bool DS1307_Write(uint8_t address, uint8_t data) 
{ 
   bool result; 
   TWI_Start(); 
   result=TWI_WriteByte((DS_SLA|TW_WRITE)); 
   if (result==false) return false; 
   result=TWI_WriteByte(address); 
   if (result==false) return false; 
   result=TWI_WriteByte(data); 
   if (result==false) return false; 
   TWI_Stop(); 
   return true; 
} 

bool TWI_ReadByte(uint8_t *data, bool ack) 
{ 
   if(ack==false) 
   { 
      /* 
      Returns NACK after reception 
      signals slave to stop giving data 
      used for last read byte 
      */ 
      TWCR&=~(1<<TWEA); 
   } 
   else 
   { 
      // Return ACK after reception 
      TWCR|=(1<<TWEA); 
   } 
   //Start data reception and wait for finish 
   TWCR|=(1<<TWINT); 
   while(!(TWCR&(1<<TWINT))); 
   /* 
   Check if completed successfully one of statuses: DATA received 
   and ACK returned, DATA received and NACK returned 
   */ 
   if ((TW_STATUS==TW_MR_DATA_ACK)||(TW_STATUS==TW_MR_DATA_NACK)) 
   { 
      *data=TWDR; 
      return true; 
   } 
   else 
   { 
      return false; 
   } 
} 

bool DS1307_Read(uint8_t address, uint8_t *data) 
{ 
   bool result; 
   TWI_Start(); 
   result=TWI_WriteByte((DS_SLA|TW_WRITE)); 
   if (result==false) return false; 
   result=TWI_WriteByte(address); 
   if (result==false) return false; 
   TWI_Start(); 
   result=TWI_WriteByte((DS_SLA|TW_READ)); 
   if (result==false) return false; 
   result=TWI_ReadByte(data,false); 
   if (result==false) return false; 
   TWI_Stop(); 
   return true; 
} 

void DS1307_Unlock(bool onoff) 
{ 
   uint8_t unlock; 
   if (onoff==true) 
   { 
      DS1307_Read(0x00,&unlock); 
      unlock&=~(1<<DS_CH); 
      DS1307_Write(0x00,unlock); 
   } 
   else 
   { 
      DS1307_Read(0x00,&unlock); 
      unlock|=(1<<DS_CH); 
      DS1307_Write(0x00,unlock); 
   } 
} 

void DS1307_24hMode() 
{ 
   uint8_t mode; 
   DS1307_Read(0x02,&mode); 
   mode&=~(1<<DS_24H); 
   DS1307_Write(0x02,mode); 
} 

void DS1307_Set(uint8_t hour, uint8_t min, uint8_t sec, uint8_t day, uint8_t date, uint8_t month, uint8_t year) 
{ 
   DS1307_Write(0x00,((sec/10)<<4)+(sec%10)); //seconds=20 //0-59 
   DS1307_Write(0x01,((min/10)<<4)+(min%10)); //minute=19 //0-59 
   DS1307_Write(0x02,((hour/10)<<4)+(hour%10)); //hour= 18 //0-23 
   DS1307_Write(0x03,day); //day=6 //1- Sunday, 2 Monday,... 1-7 
   DS1307_Write(0x04,((date/10)<<4)+(date%10)); //date=8 //1-31 
   DS1307_Write(0x05,((month/10)<<4)+(month%10)); //month=2 //1-12 
   DS1307_Write(0x06,((year/10)<<4)+(year%10)); //year=13 //0- 2000, 01- 2001,... 0-99 
}

char*  DS1307_GetTime()
{
	DS1307_Read(0x00,&vlr);
	DS1307_Time[8]='\0';
	DS1307_Time[7]=(0x30)+(vlr & 0b00001111);
	DS1307_Time[6]=(0x30)+((vlr & 0b01110000)>>4);
	DS1307_Time[5]=':';
	DS1307_Read(0x01,&vlr);
	DS1307_Time[4]=(0x30)+(vlr & 0b00001111);
	DS1307_Time[3]=(0x30)+((vlr & 0b01110000)>>4);
	DS1307_Time[2]=':';
	DS1307_Read(0x02,&vlr);
	DS1307_Time[1]=(0x30)+(vlr & 0b00001111);
	DS1307_Time[0]=(0x30)+((vlr & 0b00110000)>>4);
	return DS1307_Time;	
}

uint8_t DS1307_GetHour()
{
	DS1307_Read(0x02,&vlr);	
	uint8_t ret = 0;
	ret = ( ((vlr & 0b00110000)>>4)*10 ) + (vlr & 0b00001111);	
	return ret;
}

uint8_t DS1307_GetMinute()
{
	DS1307_Read(0x01,&vlr);
	uint8_t ret = 0;
	ret = ( ((vlr & 0b01110000)>>4)*10 ) + (vlr & 0b00001111);
	return ret;
}

uint8_t DS1307_GetSecond()
{
	DS1307_Read(0x00,&vlr);
	uint8_t ret = 0;
	ret = ( ((vlr & 0b01110000)>>4)*10 ) + (vlr & 0b00001111);
	return ret;
}
