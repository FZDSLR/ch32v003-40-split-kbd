#ifndef _WS2812B_DATA_H
#define _WS2812B_DATA_H

#define LED_NUM 6

#include <stdint.h>

void update_ws2812b_hex_data(uint8_t index,uint32_t hex);
uint32_t get_ws2812b_hex_data(uint8_t index);
int is_ws2812_in_use();
void ws2812b_dma_init();
void ws2812b_start_sending(int num);

#endif
