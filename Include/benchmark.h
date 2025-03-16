
/* UART peripheral address */
#define RME_A7A_UART_CONTROL      (*((volatile unsigned int*)(0xE0001000)))
#define RME_A7A_UART_MODE         (*((volatile unsigned int*)(0xE0001004)))
#define RME_A7A_UART_BRGEN        (*((volatile unsigned int*)(0xE0001018)))
#define RME_A7A_UART_STATUS       (*((volatile unsigned int*)(0xE000102C)))
#define RME_A7A_UART_FIFO         (*((volatile unsigned int*)(0xE0001030)))
#define RME_A7A_UART_BRDIV        (*((volatile unsigned int*)(0xE0001034)))
#define RME_A7A_UART_STATUS_TXE   (1U<<3)
//#define RME_DBG_S(STR)                              RME_Str_Print((const signed char*)(STR))


#define RME_A7A_PUTCHAR(CHAR) \
do \
{ \
    while((RME_A7A_UART_STATUS&RME_A7A_UART_STATUS_TXE)==0); \
	RME_A7A_UART_FIFO=(CHAR); \
} \
while(0)
