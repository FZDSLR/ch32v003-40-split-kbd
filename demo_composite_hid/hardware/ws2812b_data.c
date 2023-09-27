#include <stdio.h>
#include <stdint.h>


#include "ch32v003fun.h"
#define WS2812DMA_IMPLEMENTATION
#define WSRBG //For WS2816C's.
// #define WSGRB // For SK6805-EC15
#define NR_LEDS 8
#include "ws2812b_dma_spi_led_driver.h"
#include "ws2812b_data.h"

uint32_t led_buf_data[LED_NUM]={0};

uint32_t WS2812BLEDCallback( int ledno )
{
	return get_ws2812b_hex_data(ledno); // Where "tween" is a value from 0 ... 255
}

int is_ws2812_in_use(){
    return WS2812BLEDInUse;
}

void update_ws2812b_hex_data(uint8_t index,uint32_t hex){
    if (index>=LED_NUM) {
        return;
    }
    led_buf_data[index]=hex;
}

uint32_t get_ws2812b_hex_data(uint8_t index){
    if (index>=LED_NUM) {
        return 0;
    }
    return led_buf_data[index];
}

void ws2812b_dma_init(){
    WS2812BDMAInit();
}

void ws2812b_start_sending(int num){
    WS2812BDMAStart(num);
}
