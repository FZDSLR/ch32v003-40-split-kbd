#ifndef _KZ_DEVICE_WS2812_STRUCT_H
#define _KZ_DEVICE_WS2812_STRUCT_H

#include "kz_ticker.h"
#include "kz_device_basic_struct.h"
#include <stdint.h>

#include "ws2812b_data.h"
// #include "ws2812b_dma_spi_led_driver.h"

typedef struct kz_subdevice_status_ws2812{
    kz_subdevice_status_t basic_output;
    uint8_t light_num;
    uint32_t update_timestamp;
    uint32_t* hex_color_array_header;
} kz_subdevice_status_ws2812_t;

void kz_subdevice_status_ws2812_update(kz_subdevice_status_t *self){
    kz_subdevice_status_ws2812_t* p=(kz_subdevice_status_ws2812_t* )self;
    uint32_t layer_bitmask=p->basic_output.parent_addr->layer_status;
    if ((!is_ws2812_in_use())&&(kz_get_timestamp32()-(p->update_timestamp)>=10)) {
        for (int i=0 ; i<(p->light_num) ; i++) {
            if (KZ_GET_LAYER_STATUS(2,layer_bitmask)) {
                *(p->hex_color_array_header+i)=0x110000;
            }
            else if (KZ_GET_LAYER_STATUS(1,layer_bitmask)) {
                *(p->hex_color_array_header+i)=0x110011;
            }
            else {
            *(p->hex_color_array_header+i)=0x111111;
            }
        }
        for (int i=0 ; i<(p->light_num) ; i++) {
            update_ws2812b_hex_data(i,*(p->hex_color_array_header+i));
        }
        ws2812b_start_sending( p->light_num );
         // ws2812b_start_sending( 4 );
        p->update_timestamp=kz_get_timestamp32();
    }
}

#endif
