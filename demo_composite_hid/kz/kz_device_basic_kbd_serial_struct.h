#ifndef _KZ_DEVICE_BASIC_KBD_SERIAL_H
#define _KZ_DEVICE_BASIC_KBD_SERIAL_H

#include "kz_device_basic_kbd_struct.h"
#include "uart.h"
#include <stdint.h>
#include <string.h>

void kz_basickbd_serial_scan(kz_subdevice_input_t* self){
    kz_subdevice_input_basickbd_t* p=(kz_subdevice_input_basickbd_t* )self;
    // uint8_t rows=p->rows;
    // uint8_t cols=p->cols;
    // uint8_t update_flag=0;
    // uint8_t buf[3];
    read_uart_array_buf(p->scan_stat_array);
    // if (memcmp(buf, p->scan_buf_array, 3)) {
    //     update_flag=1;
    // }
    // for (int i=0; i<((rows*cols+7)/8);i++) {
    //     // if (p->scan_buf_array[i]!=addr[i]) { }
    // }
    // if (update_flag) {
    //     memcpy(p->scan_stat_array,p->scan_buf_array,(rows*cols+7)/8);
    //     p->scan_timestamp=kz_get_timestamp16();
    // }
}

#endif
