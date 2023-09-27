#include "ch32v003fun.h"
#include "kz_ticker.h"
#include "rv003usb.h"
#include "systick.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "RIngBuffer/ring_buffer.h"

#define BUFFER_SIZE_6KEY 80

typedef struct usb_hid_output{
    volatile uint32_t last_tick;
    volatile uint8_t connected;
    ring_buffer fifo_6key;
    uint8_t fifo_buf_6key[BUFFER_SIZE_6KEY];
    uint8_t report_buff_6key[2][8];
    uint8_t last_frame_6key[8];
    uint8_t send_index_6key;
    uint8_t is_sendover_6key;
} usb_hid_output_t ;

static usb_hid_output_t output1={0};

uint8_t pkg_num=0;

void read_to_usb_temp_6key(uint8_t buf_6key[8]){
    if ((uint8_t)(memcmp(buf_6key,output1.last_frame_6key,8))&&(output1.connected)) {
        pkg_num++;
        printf("p\n");
        // printf("new %d\n",pkg_num);
        memcpy(output1.last_frame_6key, buf_6key, 8);
        uint8_t debug=RB_Write_String(&output1.fifo_6key, buf_6key, 8);
        if (!debug) {
            printf("e1\n");
        }
    }
}

uint8_t is_usb_avaliable(){return output1.connected;}

void usb_process(){
    if (!output1.connected) {
        return;
    }
    if ((output1.connected)&&((kz_get_timestamp32()-output1.last_tick)>100)) {
        output1.connected=0;
        printf("usb disconnected\n");
        memset(output1.report_buff_6key,0,sizeof(output1.report_buff_6key));
        memset(output1.last_frame_6key,0,sizeof(output1.last_frame_6key));
        RB_Init(&output1.fifo_6key, output1.fifo_buf_6key, BUFFER_SIZE_6KEY);
        return;
    }
    if (output1.is_sendover_6key) {
        if (RB_Get_Length(&output1.fifo_6key)>=8) {
            uint8_t index=((~output1.send_index_6key)&0x01);
            RB_Read_String(&output1.fifo_6key,output1.report_buff_6key[index],8,1);
            // RB_Delete(&output1.fifo_6key,8);
            output1.send_index_6key=index;
            output1.is_sendover_6key=0;
            printf("ro\n");
        }
    }
}
// void read_usb_frame(uint8_t buf_6key[8]){
//     if (RB_Get_Length(&output1.fifo_6key)>=8) {
//         RB_Read_String(&output1.fifo_6key,usbkbd_active,8);
//         RB_Delete(&usbkbd_rb,8);
//         printf("ro\n");
//     }
// }

void generate_kbd_8bit_keycode(uint8_t buff_29key[29],uint8_t buff_8key[8]){
    uint8_t used_6key=0;
    memset(buff_8key, 0, 8);

    for (int i=4; i<232; i++) {
        if ((buff_29key[i/8]>>(i%8))&0x01) {
            // printf("actived %d\n",i);
            if (i<224) {
                if (used_6key<6) {
                    buff_8key[used_6key+2]=i;
                    used_6key++;
                }else {

                }
            }
            else {
                buff_8key[0]|=(0x01<<(i-224));
            }
        }
    }
}

void usb_handle_user_in_request(struct usb_endpoint* e, uint8_t* scratchpad, int endp, uint32_t sendtok, struct rv003usb_internal* ist)
{
    output1.connected=1;
    output1.last_tick=kz_get_timestamp32();
    if (endp == 1) {
        // Mouse (4 bytes)
        // static int i;
        static uint8_t tsajoystick[4] = { 0x00, 0x00, 0x00, 0x00 };
        // i++;
        // int mode = i >> 5;
        //
        // // Move the mouse right, down, left and up in a square.
        // switch (mode & 3) {
        // case 0:
        //     tsajoystick[1] = 1;
        //     tsajoystick[2] = 0;
        //     break;
        // case 1:
        //     tsajoystick[1] = 0;
        //     tsajoystick[2] = 1;
        //     break;
        // case 2:
        //     tsajoystick[1] = -1;
        //     tsajoystick[2] = 0;
        //     break;
        // case 3:
        //     tsajoystick[1] = 0;
        //     tsajoystick[2] = -1;
        //     break;
        // }
        usb_send_data(tsajoystick, 4, 0, sendtok);
    } else if (endp == 2) {
        // Keyboard (8 bytes)
        // static int i;
        // static uint8_t tsajoystick[8] = { 0x00 };

        usb_send_data(output1.report_buff_6key[output1.send_index_6key], 8, 0, sendtok);
        output1.is_sendover_6key=1;
        // i++;

        // Press the 'b' button every second or so.
        // if ((i & 0x7f) == 0) {
        //     tsajoystick[4] = 5;
        // } else {
        //     tsajoystick[4] = 0;
        // }
    } else {
        // If it's a control transfer, empty it.
        usb_send_empty(sendtok);
    }
}

void usb_init(){
    RB_Init(&output1.fifo_6key, output1.fifo_buf_6key, BUFFER_SIZE_6KEY);
    usb_setup();
}


