#define on(adress,bit) ( adress|=(1<<bit))
#define off(adress,bit) ( adress&=~(1<<bit))
#define isOn(adress,bit) (adress&(1<<bit))