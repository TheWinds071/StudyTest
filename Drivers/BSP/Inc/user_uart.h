#ifndef __USER_UART_H__
#define __USER_UART_H__

#include "ring_buffer.h"
#include "usart.h"

#define USART_RECV_BUF_SIZE 500
#define USART_SEND_BUF_SIZE 500


typedef struct
{
    UART_HandleTypeDef *huartX;
    RingBufferTypeDef *sendBuf;
    RingBufferTypeDef *recvBuf;
} Usart_DataTypeDef;

extern Usart_DataTypeDef FSUS_Usart;
extern uint8_t uart2_rx_byte;

void User_Uart_Init(UART_HandleTypeDef *huartx);
void Usart_SendAll(Usart_DataTypeDef *usart);
void User_Uart_Process(void);
#endif
