#ifndef _KZ_DEVICE_INIT_H
#define _KZ_DEVICE_INIT_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define KZ_TAPPING_TERM 140

#include "kz_device_basic_kbd_gpio_struct.h"
#include "kz_device_basic_kbd_struct.h"
#include "kz_device_basic_kbd_serial_struct.h"
#include "kz_device_basic_struct.h"
#include "kz_device_ws2812_struct.h"

#include "kz_user_keymap.h"
#include "systick.h"
#include "usb_handler.h"

uint32_t tick_count=0;
// uint32_t scan_time=0;

#define HANDEDNESS_LEFT 0
#define HANDEDNESS_RIGHT 1
#define HANDEDNESS HANDEDNESS_RIGHT

extern kz_device_t kz_main_device;

const uint8_t rowpins[]={C7,C5,C4,C3};

#if HANDEDNESS==HANDEDNESS_LEFT
const uint8_t colpins[]={D2,A1,A2,C0,C2,C1};
#define KEYMAP_GPIO kz_user_kbd_array1
#define KEYMAP_SERIAL kz_user_kbd_array2
#elif HANDEDNESS==HANDEDNESS_RIGHT
const uint8_t colpins[]={C0,C2,C1,D2,A1,A2};
#define KEYMAP_GPIO kz_user_kbd_array2
#define KEYMAP_SERIAL kz_user_kbd_array1
#endif

KZ_BASICKBD_CREATE_STATIC_ARRAYS(kbd_gpio,4,6);

kz_subdevice_input_basickbd_gpio_t kbd_gpio={{{{
    &kz_basickbd_gpio_init, //初始化
    &kz_basickbd_gpio_scan, //扫描
    &kz_basickbd_update}, //更新
    &kz_main_device, //母设备地址
    0, //上一次活动时间戳
    0,
    0},
    KZ_BASICKBD_ADDITION_THINGS(kbd_gpio,4,6,KEYMAP_GPIO)
},
    rowpins,
    colpins,
    DIODE_COL2ROW};


KZ_BASICKBD_CREATE_STATIC_ARRAYS(kbd_serial,4,6);

kz_subdevice_input_basickbd_t kbd_serial={{{
    0, //初始化
    &kz_basickbd_serial_scan, //扫描
    &kz_basickbd_update}, //更新
    &kz_main_device, //母设备地址
    0, //上一次活动时间戳
    0,
    0},
    KZ_BASICKBD_ADDITION_THINGS(kbd_serial,4,6,KEYMAP_SERIAL)
};

//更新函数 母设备地址 LED数量 led数据数组
uint32_t kz_ws2812_buf[6]={0};
kz_subdevice_status_ws2812_t status_ws2812={{{&kz_subdevice_status_ws2812_update},&kz_main_device},6,0,kz_ws2812_buf};

kz_subdevice_input_t* kz_inputlist[2]={(kz_subdevice_input_t *)&kbd_gpio,(kz_subdevice_input_t *)&kbd_serial};
kz_subdevice_output_t* kz_outputlist[1]={0};
kz_subdevice_status_t* kz_statuslist[1]={(kz_subdevice_status_t *)&status_ws2812};
// kz_subdevice_output_ws2812_t kz_ws2812;

kz_device_t kz_main_device={0,kz_inputlist,2,kz_outputlist,0,kz_statuslist,1,0,0,MODE_MASTER,{{0}}};

// kz_subdevice_status_t* ws2812_p=(kz_subdevice_status_t *)&device;

void scan1(kz_subdevice_input_t *self){
    printf("scan1\n");
}

// void output1(kz_subdevice_output_t *self){
//     printf("output1\n");
// }
//
// void output2(kz_subdevice_output_t *self){
//     printf("output2,%d\n",((kz_subdevice_output_ws2812_t *)self)->index);
//     ((kz_subdevice_output_ws2812_t *)self)->index++;
// }

void kz_subdevice_init(){
    // kz_statuslist[0]=ws2812_p;
    kz_inputlist[0]->vtbl.init(kz_inputlist[0]);
    // kz_outputlist[0].vtbl.do_report=&output1;
    // kz_ws2812.basic_output.vtbl.do_report=&output2;
    // kz_ws2812.index=0;
}

void kz_process(){
    if (is_usb_avaliable()) {
        kz_main_device.mode=MODE_MASTER;
    }else {
        kz_main_device.mode=MODE_SLAVE;
    }
    // if(GET_SYSTICK()-scan_time>=1){
        kz_inputlist[0]->vtbl.scan(kz_inputlist[0]);
        kz_inputlist[1]->vtbl.scan(kz_inputlist[1]);
// /*        scan_time=GET_SYSTICK();
//    */ }
    memset(kz_main_device.inputbuf.basickbd_buff,0,29);
    // clear_usb_temp();
    uint8_t o_mode,o_default_layer;
    uint32_t o_layer_status;
    read_uart_device_data(&o_default_layer,&o_layer_status,&o_mode);
    if (kz_main_device.mode==MODE_MASTER) {
        kz_inputlist[0]->vtbl.update(kz_inputlist[0]);
        kz_inputlist[1]->vtbl.update(kz_inputlist[1]);
    } else if (o_mode==MODE_MASTER&&kz_main_device.mode==MODE_SLAVE) {
        kz_main_device.defalut_layer=o_default_layer;
        kz_main_device.layer_status=o_layer_status;
        // printf("%ld\n",o_layer_status);
    };
    // printf("mode:%d,%d\n",o_mode,kz_main_device.mode);
    // printf("layer:%ld,%ld\n",kz_main_device.layer_status,o_layer_status);
    uint8_t buf_6key[8];
    generate_kbd_8bit_keycode(kz_main_device.inputbuf.basickbd_buff,buf_6key);
    read_to_usb_temp_6key(buf_6key);
    usb_process();
    kz_statuslist[0]->vtbl.update(kz_statuslist[0]);
    if (GET_SYSTICK()-tick_count>=5) {
        // printf("%ld\n",process_count);
        // process_count=0;
        char str[]={0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xef};
        memcpy(&str[2], kbd_gpio.basic_output.scan_stat_array, 3);
        memcpy(&str[1], &kz_main_device.mode, 1);
        memcpy(&str[5], &kz_main_device.defalut_layer, 1);
        memcpy(&str[6], &kz_main_device.layer_status, 4);
        uart_write(str, 11);
        tick_count=GET_SYSTICK();
    }

    // update_usb_temp();
    // read_usb_frame();
    // generate_kbd_8bit_keycode();
    // kz_outputlist[0].vtbl.do_report(&kz_outputlist[0]);
    // ws2812_p->vtbl.do_report((kz_subdevice_output_t *)&kz_ws2812);

}

#endif
