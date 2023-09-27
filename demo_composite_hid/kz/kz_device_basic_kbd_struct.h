#ifndef _KZ_DEVICE_BASIC_KBD_STRUCT_H
#define _KZ_DEVICE_BASIC_KBD_STRUCT_H

#include <stdint.h>
#include <stdio.h>
#include "kz_device_basic_struct.h"
#include "kz_keycode.h"
#include "kz_ticker.h"
#include "tinyusb_hid.h"
#include <string.h>
#include "usb_handler.h"

#ifndef KZ_TAPPING_TERM
#define KZ_TAPPING_TERM 160
#endif

typedef struct kz_subdevice_input_basickbd{
    kz_subdevice_input_t basic_output;
    const uint8_t rows;
    const uint8_t cols;
    uint16_t* status_timestamp_array; //时间戳
    uint8_t* status_array; //状态，八位
    uint16_t* active_keycode; //活动键码
    uint8_t* scan_stat_array; //bitmask
    uint8_t* scan_buf_array; //bitmask
    uint16_t scan_timestamp;
    uint8_t scan_newstat_flag;
    const uint16_t* keymap_array;
    const uint8_t keymap_layers;
    uint8_t keycode_out_buff[(231+7)/8];
} kz_subdevice_input_basickbd_t;

enum{
    KEY_LONG_RELEASED=0b0000,
    KEY_SHORT_RELEASED,
    KEY_ENTERING_RELEASED,
    KEY_SHORT_PRESSED=0b1000,
    KEY_LONG_PRESSED,
    KEY_ENTERING_PRESSED,
    KEY_INVALID=0b1111,
};

#define KZ_KEYSTAT_PRESSED(X) ((X>>3)&0x01)

// void kz_gpio_init();

void kz_basickbd_init(kz_subdevice_input_t *self){
    // printf("output2,%d\n",((kz_subdevice_output_basickbd_t *)self)->rows);
    // GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_D, 0), GPIO_pinMode_O_pushPull, GPIO_Speed_10MHz);
}

#define kz_bitmap_init(name,rows,cols) uint8_t name[(rows*cols+7)/8]={0}

#define kz_readbit(matrix, row, col,total_cols) (((matrix)[(((row)*total_cols+col) / 8)] >> (((row)*total_cols+col) % 8)) & 0x01)

#define kz_setbit(matrix, row, col,total_cols) ((matrix)[(((row)*total_cols+col) / 8)] |= (0x01 << (((row)*total_cols+col) % 8)))

#define kz_resetbit(matrix, row, col,total_cols) ((matrix)[(((row)*total_cols+col) / 8)] &= (~(0x01 << (((row)*total_cols+col) % 8))))

void kz_keycode_decode(uint16_t keycode, uint8_t status,uint8_t* keycode_out_buff,uint8_t* next_status,uint32_t* layer_status,uint8_t* defalut_layer, uint8_t* end_flag){
    uint8_t nowstatus=(status&0x0f);
    uint8_t laststatus=(status>>4);
    uint8_t nextstatus=KEY_INVALID;
    uint8_t endflag=0;
    if (!KZ_KEYCODE_HIGH(keycode,8)) { // 普通键码
        if (KZ_KEYSTAT_PRESSED(nowstatus)) {
            keycode_out_buff[keycode/8]|=(0x01<<(keycode%8));
            // printf("%d ",keycode);
            nextstatus = KEY_LONG_PRESSED;
        } else {
            nextstatus = KEY_LONG_RELEASED;
            // printf("r %d ",keycode);
            endflag=1;
        }
    }
    else if ((KZ_KEYCODE_HIGH(keycode,1))) {
        uint8_t layer=(keycode>>8)&0x0f;
        uint8_t layer_cmd=keycode>>12;
        switch (layer_cmd){
            case CMD_LAYER_MO:
                if (nowstatus==KEY_ENTERING_PRESSED) {
                    nextstatus = KEY_LONG_PRESSED;
                    KZ_SET_LAYER_STATUS(*layer_status,(keycode&0xff));
                } else if (nowstatus==KEY_ENTERING_RELEASED) {
                    nextstatus = KEY_LONG_RELEASED;
                    KZ_RESET_LAYER_STATUS(*layer_status,(keycode&0xff));
                    endflag=1;
                }
                break;
            case CMD_LAYER_LT:
                if (nowstatus==KEY_ENTERING_PRESSED) {
                    nextstatus = KEY_SHORT_PRESSED;
                    // printf("LTp ");
                }
                else if (laststatus==KEY_ENTERING_RELEASED){
                    endflag=1;
                    // printf("LTe ");
                }
                else if((nowstatus==KEY_LONG_PRESSED)&&(laststatus==KEY_SHORT_PRESSED)){
                    // printf("LTe ");
                    KZ_SET_LAYER_STATUS(*layer_status,layer);
                }
                else if ((nowstatus==KEY_ENTERING_RELEASED)&&(laststatus==KEY_SHORT_PRESSED)) {
                    // nextstatus = KEY_LONG_RELEASED;
                    keycode_out_buff[(keycode&0xff)/8]|=(0x01<<((keycode&0xff)%8));
                    // endflag=1;
                    // printf("LTs ");
                }
                else if ((nowstatus==KEY_ENTERING_RELEASED)&&(laststatus==KEY_LONG_PRESSED)) {
                    nextstatus = KEY_LONG_RELEASED;
                    endflag=1;
                    KZ_RESET_LAYER_STATUS(*layer_status,layer);
                    // printf("LTl ");
                }

                else{
                    // printf("LTN %d %d\n",laststatus,nowstatus);
                }
                break;
            case CMD_LAYER_TG:
                if (nowstatus==KEY_ENTERING_PRESSED) {
                    nextstatus = KEY_LONG_PRESSED;
                    if (KZ_GET_LAYER_STATUS(keycode&0xff, *layer_status)) {
                        KZ_RESET_LAYER_STATUS(*layer_status,(keycode&0xff));
                    }
                    else {
                        KZ_SET_LAYER_STATUS(*layer_status,(keycode&0xff));
                    }
                } else if (nowstatus==KEY_ENTERING_RELEASED) {
                    nextstatus = KEY_LONG_RELEASED;
                    endflag=1;
                }
                break;
            case CMD_LAYER_LM:
                if (KZ_KEYSTAT_PRESSED(nowstatus)) {
                    uint8_t mask=keycode&0xff;
                    for (int i=0; i<8; i++) {
                        if((mask>>i)&0x01){keycode_out_buff[(224+i)/8]|=(0x01<<((224+i)%8));}
                    }
                    printf("%d ",keycode);
                    nextstatus = KEY_LONG_PRESSED;
                }
                if (nowstatus==KEY_ENTERING_PRESSED) {
                    KZ_SET_LAYER_STATUS(*layer_status,layer);
                    nextstatus = KEY_LONG_PRESSED;
                } else if (nowstatus==KEY_ENTERING_RELEASED) {
                    KZ_RESET_LAYER_STATUS(*layer_status,layer);
                    nextstatus = KEY_LONG_RELEASED;
                    endflag=1;
                }
                break;
            default:
                endflag=1;
                // printf("LTN %d %d\n",laststatus,nowstatus);
                break;
        }
    } else {
        endflag=1;
    }

    if (nextstatus!=KEY_INVALID) {
        *next_status=nextstatus;
    }
    *end_flag=endflag;
}

void kz_basickbd_update(kz_subdevice_input_t *self){
    kz_subdevice_input_basickbd_t* p=(kz_subdevice_input_basickbd_t* )self;
    uint8_t cols=p->cols;
    uint8_t rows=p->rows;
    uint16_t timestamp16_now=kz_get_timestamp16();
    uint8_t stat_update_flag=0;
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            //首次按下，压入键码
            if ((kz_readbit(p->scan_stat_array,i,j,cols))&&(!KZ_KEYSTAT_PRESSED(p->status_array[i*cols+j]))) { //
                // printf("keycode process\n");

                //检查额外启用层是否有有效键码
                uint8_t valid_keycode_flag=0;
                for (int k=p->keymap_layers-1; k>=0; k--) {
                    if(KZ_GET_LAYER_STATUS(k,p->basic_output.parent_addr->layer_status)){
                        uint32_t keycode=p->keymap_array[cols*rows*k+(i*cols+j)];
                        if (keycode!=KC_TRANSPARENT) { //不是 NO(就是0) 或 TRANSPARENT
                            p->active_keycode[i*cols+j]=p->keymap_array[cols*rows*k+(i*cols+j)];
                            printf("pressed %d\n",p->active_keycode[i*cols+j]);
                            valid_keycode_flag=1;
                            break;
                        }
                    }
                }
                //检查默认层是否有有效键码
                if (!valid_keycode_flag) {
                    uint8_t defalut_layer=p->basic_output.parent_addr->defalut_layer;
                    if (defalut_layer<=p->keymap_layers) {
                        if (p->keymap_array[cols*rows*defalut_layer+(i*cols+j)]==KC_TRANSPARENT) {
                            p->active_keycode[i*cols+j]=0;
                        } else{
                            p->active_keycode[i*cols+j]=p->keymap_array[cols*rows*defalut_layer+(i*cols+j)];
                            printf("pressed %d\n",p->active_keycode[i*cols+j]);
                        }
                    } else {
                        p->active_keycode[i*cols+j]=0;
                        // printf("pressed %d\n",p->active_keycode[i*cols+j]);
                    }
                }
                // p->status_array[i*cols+j]=((p->status_array[i*cols+j]<<4)|KEY_SHORT_PRESSED);
                // p->status_timestamp_array[i*cols+j]=kz_get_timestamp32();
                // printf("update:diff on %d,%d\n",i,j);
            }
            // 压入键码结束

            //活动键码非0，即进入状态判断


            if (p->active_keycode[i*cols+j]) {
                //按下状态bitmask与状态数组当前按下位不同
                if ((kz_readbit(p->scan_stat_array,i,j,cols))!=(KZ_KEYSTAT_PRESSED(p->status_array[i*cols+j]))) {
                    switch (kz_readbit(p->scan_stat_array,i,j,cols)) {
                        case 0: // 抬起
                            p->status_array[i*cols+j]=((p->status_array[i*cols+j]<<4)|KEY_ENTERING_RELEASED);
                            p->status_timestamp_array[i*cols+j]=timestamp16_now;
                            // printf("entering released\n");
                            // printf("status:%d\n",p->status_array[i*cols+j]);
                            stat_update_flag=1;
                            break;
                        default: // 按下
                            p->status_array[i*cols+j]=((p->status_array[i*cols+j]<<4)|KEY_ENTERING_PRESSED);
                            p->status_timestamp_array[i*cols+j]=timestamp16_now;
                            // printf("entering pressed\n");
                            // printf("status:%d\n",p->status_array[i*cols+j]);
                            stat_update_flag=1;
                            break;
                    }
                } else { //相同，则根据时间戳判断长短按状态是否改变

                    uint16_t time_diff=timestamp16_now-(p->status_timestamp_array[i*cols+j]);
                    switch ((p->status_array[i*cols+j])&0x0F) {
                        case KEY_LONG_PRESSED:
                            p->status_array[i*cols+j]=((KEY_LONG_PRESSED<<4)|KEY_LONG_PRESSED);
                            break;
                        case KEY_LONG_RELEASED:
                            p->status_array[i*cols+j]=((KEY_LONG_RELEASED<<4)|KEY_LONG_RELEASED);
                            break;
                        case KEY_ENTERING_PRESSED:
                            p->status_array[i*cols+j]=((KEY_ENTERING_PRESSED<<4)|KEY_LONG_PRESSED);
                            // printf("entering pressed 2\n");
                            stat_update_flag=1;
                            break;
                        case KEY_ENTERING_RELEASED:
                            p->status_array[i*cols+j]=((KEY_ENTERING_RELEASED<<4)|KEY_LONG_RELEASED);
                            stat_update_flag=1;
                            break;
                        case KEY_SHORT_PRESSED:
                            // printf("diff=%d/n",time_diff);
                            if (time_diff>KZ_TAPPING_TERM) {
                                //状态变为长按
                                p->status_array[i*cols+j]=((p->status_array[i*cols+j]<<4)|KEY_LONG_PRESSED);
                                p->status_timestamp_array[i*cols+j]=timestamp16_now;
                                stat_update_flag=1;
                                // printf("cg longpressed %d %d %d %d\n",p->active_keycode[i*cols+j],i,j,p->status_array[i*cols+j]);
                            } else {
                                p->status_array[i*cols+j]=((KEY_SHORT_PRESSED<<4)|KEY_SHORT_PRESSED);
                            }
                            break;
                        case KEY_SHORT_RELEASED:
                            if (time_diff>200) {
                                //状态变为长松
                                p->status_array[i*cols+j]=((p->status_array[i*cols+j]<<4)|KEY_LONG_RELEASED);
                                p->status_timestamp_array[i*cols+j]=timestamp16_now;
                                stat_update_flag=1;
                                // printf("cg longreleased %d %d %d\n",p->active_keycode[i*cols+j],i,j);
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }
    if (stat_update_flag) { //状态改变，更新键码
        // printf("\nnew keycode\n");
        memset(p->keycode_out_buff,0,sizeof(p->keycode_out_buff));
        for (int i=0; i<rows; i++) {
            for (int j=0; j<cols; j++) {
                if (p->active_keycode[i*cols+j]) {
                    uint8_t next_status=KEY_INVALID;
                    uint32_t layer_status=p->basic_output.parent_addr->layer_status;
                    uint8_t defalut_layer=p->basic_output.parent_addr->defalut_layer;
                    uint8_t end_flag=0;
                    printf("kc %d,%d,%d\n",p->active_keycode[i*cols+j],i,j);
                    kz_keycode_decode(p->active_keycode[i*cols+j],p->status_array[i*cols+j],p->keycode_out_buff,&next_status,&(layer_status),&(defalut_layer),&end_flag);
                    if (next_status!=KEY_INVALID) {
                        p->status_array[i*cols+j]=((p->status_array[i*cols+j])&0xf0)|(next_status&0x0f);
                    }
                    if (end_flag) {
                        p->active_keycode[i*cols+j]=0;
                    }
                    if (p->basic_output.parent_addr->mode==MODE_MASTER) {
                        p->basic_output.parent_addr->layer_status=layer_status;
                        p->basic_output.parent_addr->defalut_layer=defalut_layer;
                    }
                }
            }
        }
    }
    for (int i=0; i<29; i++) {
        p->basic_output.parent_addr->inputbuf.basickbd_buff[i]|=p->keycode_out_buff[i];
    }
}

#define KZ_BASICKBD_CREATE_STATIC_ARRAYS(Dev,Rows,Cols) uint16_t Dev##_##status_timestamp_array[Rows*Cols]={0};\
uint8_t Dev##_##status_array[Rows*Cols]={0};\
uint16_t Dev##_##active_keycode[Rows*Cols]={0};\
uint8_t Dev##_##scan_stat_array[(Rows*Cols+7)/8]={0};\
uint8_t Dev##_##scan_buf_array[(Rows*Cols+7)/8]={0};

#define KZ_BASICKBD_ADDITION_THINGS(Dev,Rows,Cols,Keymap) Rows,\
Cols,\
Dev##_##status_timestamp_array,\
Dev##_##status_array,\
Dev##_##active_keycode,\
Dev##_##scan_stat_array,\
Dev##_##scan_buf_array,\
0,\
0,\
&Keymap[0][0],\
(sizeof(Keymap)/sizeof(Keymap[0])),\
{0},

#endif
