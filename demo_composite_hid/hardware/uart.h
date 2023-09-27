#ifndef _UART_H
#define _UART_H

#include <stdint.h>

void uart_init();
void uart_write(const char *buf, int size);
void read_uart_array_buf(uint8_t* addr);
void read_uart_device_data(uint8_t* default_layer,uint32_t* layer_mask,uint8_t* mode);

#endif
