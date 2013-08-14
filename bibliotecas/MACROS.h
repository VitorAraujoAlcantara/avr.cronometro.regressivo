// MACROS
#define set_bit(adress,bit)	(adress|=(1<<bit))	// facilidades para o acesso aos pinos
#define clr_bit(adress,bit) (adress&=~(1<<bit))	// (PORTx,Pxx)
#define tst_bit(adress,bit) (adress&(1<<bit))	// Leitura dos I/Os (PINx,Pxx)
#define cpl_bit(adress,bit)	(adress^=(1<<bit))	// complementa o valor do bit