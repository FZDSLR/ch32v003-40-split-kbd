#ifndef _KZ_DEVICE_BASIC_KBD_GPIO_H
#define _KZ_DEVICE_BASIC_KBD_GPIO_H

#include "kz_device_basic_kbd_struct.h"
#include "ch32v003_GPIO_branchless.h"
#include "kz_device_basic_struct.h"
#include "kz_gpio.h"
#include "kz_ticker.h"
#include <stdint.h>
#include <stdio.h>
#include "string.h"

#define KZ_DELAY_US(X) Delay_Us(X)

enum{
    DIODE_COL2ROW,
    DIODE_ROW2COL,
};

typedef struct kz_subdevice_input_basickbd_gpio{
    kz_subdevice_input_basickbd_t basic_output;
    const uint8_t* row_pins;
    const uint8_t* col_pins;
    uint8_t mode;
} kz_subdevice_input_basickbd_gpio_t;

void kz_basickbd_gpio_init(kz_subdevice_input_t* self){
    kz_subdevice_input_basickbd_gpio_t* p=(kz_subdevice_input_basickbd_gpio_t* )self;
    uint8_t rows=p->basic_output.rows;
    uint8_t cols=p->basic_output.cols;

    KZ_ENABLE_GPIO_PORT_

    switch (p->mode) {
        case DIODE_COL2ROW:
            for (uint8_t i=0; i<rows; i++) {
                KZ_GPIO_SET_INPUT(*(p->row_pins+i));
                //*(p->row_pins+i)
                //GPIOIN
            }

            for (uint8_t i=0; i<cols; i++) {
                KZ_GPIO_SET_OUTPUT(*(p->col_pins+i));
                KZ_GPIO_LOW(*(p->col_pins+i));
                //GPIOout
            }
            break;
        case DIODE_ROW2COL:
            for (uint8_t i=0; i<rows; i++) {
                KZ_GPIO_SET_OUTPUT(*((p->row_pins)+i));
                KZ_GPIO_LOW(*((p->row_pins)+i));
                //GPIOout
            }
            for (uint8_t i=0; i<cols; i++) {
                KZ_GPIO_SET_INPUT(*(p->col_pins+i));
                //GPIOIN
            }
            break;
        default:
            break;
    }
}

void kz_basickbd_gpio_scan(kz_subdevice_input_t* self){
    kz_subdevice_input_basickbd_gpio_t* p=(kz_subdevice_input_basickbd_gpio_t* )self;
    uint8_t rows=p->basic_output.rows;
    uint8_t cols=p->basic_output.cols;
    uint8_t update_flag=0;

    switch (p->mode) {
        case DIODE_COL2ROW:
            // for (uint8_t i=0; i<cols; i++) {
            //     //GPIOout all zero
            // }

            for (uint8_t i=0; i<cols; i++) {
                //GPIOout 1
                KZ_GPIO_HIGH(*(p->col_pins+i));
                KZ_DELAY_US(1);
                for (uint8_t j=0; j<rows; j++) {
                    uint8_t temp=0;
                    temp=KZ_GPIO_READ(*((p->row_pins)+j));
                    uint8_t temp2=kz_readbit(p->basic_output.scan_buf_array, j, i, cols);

                    if ((temp>0&&temp2>0)||(temp==0&&temp2==0)) {
                    }
                    else {
                        update_flag=1;
                        // printf("diff on %d,%d,%d,%d\n",j,i,temp,temp2);
                        if (temp) {
                            kz_setbit(p->basic_output.scan_buf_array, j, i, cols);
                        } else {
                            kz_resetbit(p->basic_output.scan_buf_array, j, i, cols);
                        }
                    }
                    // (temp?kz_setbit(p->basic_output.scan_buf_array, j, i, cols): kz_resetbit(p->basic_output.scan_buf_array, j, i, cols));
                    // if(temp){
                    //     printf("high=%d,%d\n",j,i);
                    // }
                    //*(p->row_pins+i)
                    //GPIOIN
                }
                KZ_GPIO_LOW(*(p->col_pins+i));
                //GPIOout 0
            }
            break;
        // case DIODE_ROW2COL:
        //     // for (uint8_t i=0; i<rows; i++) {
        //     //     //GPIOout all zero
        //     // }
        //     for (uint8_t i=0; i<rows; i++) {
        //         //GPIOout 1
        //         for (uint8_t j=0; j<cols; j++) {
        //             //GPIOIN
        //         }
        //         //GPIOout 0
        //     }
        //     break;
        default:
            break;
    }

#define JET_TIME 5
    if (update_flag) {
        p->basic_output.scan_newstat_flag=1;
        p->basic_output.scan_timestamp=kz_get_timestamp16();
    } else {
        if((p->basic_output.scan_newstat_flag)&&((kz_get_timestamp16()-p->basic_output.scan_timestamp)>=JET_TIME)){
            p->basic_output.scan_newstat_flag=0;
            memcpy(p->basic_output.scan_stat_array,p->basic_output.scan_buf_array,(rows*cols+7)/8);
            // printf("jet removed\n");
        }
    }
}

#endif
