#include "ch32v003fun.h"

#include "uart.h"
#include "usb_handler.h"
#include "ch32v003_GPIO_branchless.h"
#include "ws2812b_data.h"
#include "kz_device_init.h"
#include "systick.h"
#include <stdint.h>
#include <stdio.h>

int main()
{
    SystemInit();
    usb_init();
    uart_init();
    ws2812b_dma_init();
    kz_subdevice_init();
    printf("init\n");
    systick_init();
    //Delay_Ms(10);b
    // uint32_t process_count=0;
    while (1) {
        // Delay_Ms(1000);
        // printf("systick=%ld\n",GET_SYSTICK());
        kz_process();
        // Delay_Ms(1);
        // process_count++;
        // Delay_Ms(1);
        // char str[]="test";
        // uart_write(str,4 );
    }
}
