#include "ch32v003fun.h"
#include "ch32v003_GPIO_branchless.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "RIngBuffer/ring_buffer.h"

#define UART_BAUD_RATE_ 115200
#define OVER4DIV_ 4
#define INTEGER_DIVIDER_ (((25 * (FUNCONF_SYSTEM_CORE_CLOCK)) / ((OVER4DIV_) * (UART_BAUD_RATE_))))
#define FRACTIONAL_DIVIDER_ ((INTEGER_DIVIDER_)%100)
#define UART_BRR_ ((((INTEGER_DIVIDER_) / 100) << 4) | (((((FRACTIONAL_DIVIDER_) * ((OVER4DIV_)*4)) + 50)/100)&15))

#define UART_RCV_BUF_SIZE 64
// #define UART_SEND_BUF_SIZE 64

uint8_t uart_dma_rcv_buff[UART_RCV_BUF_SIZE]={0};
uint8_t uart_fifo_rcv_buff[UART_RCV_BUF_SIZE]={0};
// uint8_t uart_dma_send_buff[UART_SEND_BUF_SIZE]={'1','2','3'};

typedef struct uart_rcv_data{
    uint8_t uart_scan_array_buf[3];
    uint8_t default_layer;
    uint32_t layer_mask;
    uint8_t mode;
} uart_rcv_data_t;

uart_rcv_data_t uart_rcv_struct[2]={0};

// uint8_t uart_scan_array_buf[2][3]={0};
volatile uint8_t uart_array_using_buf=0;

ring_buffer fifo_uart_rcv={0};

// typedef struct uart_frame
// {
//     uint8_t head;
//     uint8_t mode;
//     uint8_t array[3];
//     uint8_t default_layer;
//     uint32_t layer_mask;
//     uint8_t tail;
// } uart_frame_t;
//
// union uart_frame_union
// {
//     uart_frame_t frame;
//     uint8_t uart_frame_bytes[sizeof(uart_frame_t)];
// };

void USART_ITConfig(USART_TypeDef *USARTx, uint16_t USART_IT, FunctionalState NewState)
{
    uint32_t usartreg = 0x00, itpos = 0x00, itmask = 0x00;
    uint32_t usartxbase = 0x00;


    usartxbase = (uint32_t)USARTx;
    usartreg = (((uint8_t)USART_IT) >> 0x05);
    itpos = USART_IT & IT_Mask;
    itmask = (((uint32_t)0x01) << itpos);

    if(usartreg == 0x01)
    {
        usartxbase += 0x0C;
    }
    else if(usartreg == 0x02)
    {
        usartxbase += 0x10;
    }
    else
    {
        usartxbase += 0x14;
    }

    if(NewState != DISABLE)
    {
        *(__IO uint32_t *)usartxbase |= itmask;
    }
    else
    {
        *(__IO uint32_t *)usartxbase &= ~itmask;
    }
}

void uart_init(){

    RB_Init(&fifo_uart_rcv, uart_fifo_rcv_buff, UART_RCV_BUF_SIZE);

    GPIO_port_enable(GPIO_port_D);

    GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_D, 5), GPIO_pinMode_O_pushPullMux, GPIO_Speed_50MHz); //  TX = D5
    GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_D, 6), GPIO_pinMode_I_pullUp, GPIO_Speed_50MHz); //  RX = D6

    RCC->APB2PCENR |= RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1;
    RCC->AHBPCENR |= RCC_AHBPeriph_DMA1;
	// 115200, 8n1.  Note if you don't specify a mode, UART remains off even when UE_Set.
	USART1->CTLR1 = USART_WordLength_8b | USART_Parity_No | USART_Mode_Tx | USART_Mode_Rx;
	USART1->CTLR2 = USART_StopBits_1;
	USART1->CTLR3 = USART_HardwareFlowControl_None;

	USART1->BRR = UART_BRR_;


    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); //打开串口接收空闲中断
    NVIC_EnableIRQ( USART1_IRQn );


    DMA1_Channel5->PADDR = (uint32_t)&USART1->DATAR;
	DMA1_Channel5->MADDR = (uint32_t)uart_dma_rcv_buff;
	DMA1_Channel5->CNTR  = UART_RCV_BUF_SIZE;
	DMA1_Channel5->CFGR  =
		DMA_M2M_Disable |
		DMA_Priority_VeryHigh |
		DMA_MemoryDataSize_Byte |
		DMA_PeripheralDataSize_Byte |
		DMA_MemoryInc_Enable |
		DMA_Mode_Normal | // OR DMA_Mode_Circular or DMA_Mode_Normal
		DMA_DIR_PeripheralSRC; //|
		// DMA_IT_TC | DMA_IT_HT; // Transmission Complete + Half Empty Interrupts.
    __set_INTSYSCR( __get_INTSYSCR() | 2 ); // Enable interrupt nesting.
    PFIC->IPRIOR[32] = 0b10000000; // Turn on preemption for USART1
    DMA1_Channel5->CFGR |= DMA_CFGR1_EN;

    USART1->CTLR3 |= (USART_DMAReq_Rx);
    USART1->CTLR1 |= CTLR1_UE_Set;
}



ITStatus USART_GetITStatus(USART_TypeDef *USARTx, uint16_t USART_IT)
{
    uint32_t bitpos = 0x00, itmask = 0x00, usartreg = 0x00;
    ITStatus bitstatus = RESET;

    usartreg = (((uint8_t)USART_IT) >> 0x05);
    itmask = USART_IT & IT_Mask;
    itmask = (uint32_t)0x01 << itmask;

    if(usartreg == 0x01)
    {
        itmask &= USARTx->CTLR1;
    }
    else if(usartreg == 0x02)
    {
        itmask &= USARTx->CTLR2;
    }
    else
    {
        itmask &= USARTx->CTLR3;
    }

    bitpos = USART_IT >> 0x08;
    bitpos = (uint32_t)0x01 << bitpos;
    bitpos &= USARTx->STATR;

    if((itmask != (uint16_t)RESET) && (bitpos != (uint16_t)RESET))
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

void USART_ClearITPendingBit(USART_TypeDef *USARTx, uint16_t USART_IT)
{
    uint16_t bitpos = 0x00, itmask = 0x00;

    bitpos = USART_IT >> 0x08;
    itmask = ((uint16_t)0x01 << (uint16_t)bitpos);
    USARTx->STATR = (uint16_t)~itmask;
}

void uart_decode(){
    while(RB_Get_Length(&fifo_uart_rcv)){
        uint8_t count=RB_Get_Length(&fifo_uart_rcv);
        if (count<8) {
            return;
        }
        uint8_t buf[11]={0};
        RB_Read_String(&fifo_uart_rcv, buf, 11, 0);
        if (buf[0]==0xfe&&buf[10]==0xef) {
            uint8_t index=((~uart_array_using_buf)&0x01);
            //第1字节：帧头
            //第2字节：主从模式
            //第3-5字节：key
            //第6字节：默认层
            //第7-10字节：激活层
            //第11字节：帧尾
            memcpy(&uart_rcv_struct[index].mode, &buf[1], 1);
            memcpy(uart_rcv_struct[index].uart_scan_array_buf, &buf[2], 3);
            memcpy(&uart_rcv_struct[index].default_layer, &buf[5], 1);
            memcpy(&uart_rcv_struct[index].layer_mask, &buf[6], 4);
            uart_array_using_buf=index;
            // printf("rcvd\n");
            RB_Delete(&fifo_uart_rcv, 11);
        } else {
            RB_Delete(&fifo_uart_rcv, 1);
        }
    }
}

void USART1_IRQHandler(void) __attribute__((interrupt));

void USART1_IRQHandler (void)
{
    // if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    // {
    //     USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    // }
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        USART1->STATR;
        USART1->DATAR;

        uint16_t count=UART_RCV_BUF_SIZE-DMA1_Channel5->CNTR;
        // printf("uartlength=%d\n",count);
        RB_Write_String(&fifo_uart_rcv, uart_dma_rcv_buff, count);
        uart_decode();
        // if (RB_Get_FreeSize(&fifo_uart_rcv)<count) {
        //
        // }
        // RB_Write_String(&fifo_uart_rcv, uart_dma_rcv_buff, count);
        // while (RB_Get_Length(&fifo_uart_rcv)>=11) {
        //     uint8_t head=0;
        //     RB_Read_Byte(&fifo_uart_rcv,&head);
        //     if (head!=0xfe) {
        //         RB_Delete(&fifo_uart_rcv, 1);
        //     } else {
        //         printf("dec\n");
        //         uint8_t decode_buf[11];
        //         RB_Read_String(&fifo_uart_rcv, decode_buf, 11);
        //         if((decode_buf[0]==0xfe)&&(decode_buf[10]==0xef)){
        //             uint8_t index=((~uart_array_using_buf)&0x01);
        //             memcpy(uart_scan_array_buf[index], &decode_buf[2], 3);
        //             uart_array_using_buf=index;
        //             RB_Delete(&fifo_uart_rcv, 11);
        //             printf("rcvd %d\n",index);
        //         } else {
        //             RB_Delete(&fifo_uart_rcv, 1);
        //         }
        //     }
        // }
        //第1字节：帧头
        //第2字节：主从模式
        //第3-5字节：key
        //第6字节：默认层
        //第7-10字节：激活层
        //第11字节：帧尾
        // if((count==11)&&(uart_dma_rcv_buff[0]==0xfe)&&(uart_dma_rcv_buff[10]==0xef)){
        //     uint8_t index=((~uart_array_using_buf)&0x01);
        //     memcpy(uart_scan_array_buf[index], &uart_dma_rcv_buff[2], 3);
        //     uart_array_using_buf=index;
        //     printf("rcvd %d\n",index);
        // }
        USART_ClearITPendingBit(USART1, USART_IT_IDLE);
        DMA1_Channel5->CFGR &= (uint16_t)(~DMA_CFGR1_EN);
        DMA1_Channel5->CNTR  = UART_RCV_BUF_SIZE;
        DMA1_Channel5->CFGR |= DMA_CFGR1_EN;
        // printf("idle\n");
    }
}

void read_uart_array_buf(uint8_t* addr){
    memcpy(addr, uart_rcv_struct[uart_array_using_buf].uart_scan_array_buf, 3);
}

void read_uart_device_data(uint8_t* default_layer,uint32_t* layer_mask,uint8_t* mode){
    uint8_t index=uart_array_using_buf;
    *default_layer=uart_rcv_struct[index].default_layer;
    *layer_mask=uart_rcv_struct[index].layer_mask;
    *mode=uart_rcv_struct[index].mode;
}

void uart_write(const char *buf, int size)
{
	for(int i = 0; i < size; i++){
	    while( !(USART1->STATR & USART_FLAG_TC));
	    USART1->DATAR = *buf++;
	}
}
