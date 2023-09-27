#ifndef _KZ_DEVICE_BASIC_STRUCT_H
#define _KZ_DEVICE_BASIC_STRUCT_H

#include <stdint.h>
#include <stdio.h>

// enum{
//     KEYBOARD,
//     MOUSE,
//     DIAL,
//     SERIAL,
// };

typedef struct kz_subdevice_input kz_subdevice_input_t;
typedef struct kz_subdevice_output kz_subdevice_output_t;
typedef struct kz_subdevice_status kz_subdevice_status_t;
// typedef struct kz_output kz_output_t;
typedef struct kz_device kz_device_t;

//输入设备虚函数表
typedef struct kz_subdevice_input_vtbl{
    void (*init)(kz_subdevice_input_t *self); //初始化
    void (*scan)(kz_subdevice_input_t *self); //扫描
    void (*update)(kz_subdevice_input_t *self); //更新键码
} kz_subdevice_input_vtbl_t;

typedef struct kz_subdevice_input{
    kz_subdevice_input_vtbl_t vtbl; //虚函数表
    kz_device_t* parent_addr; //母设备地址
    uint32_t last_active_timestamp; //上一次输入事件时间挫
    uint8_t update_flag; //更新标志
    uint8_t report_flag; //播报标志
    uint8_t max_layer; //最大层
} kz_subdevice_input_t;

typedef struct kz_subdevice_output_vtbl{
    uint8_t is_avaliable;
    void (*check_is_avaliable)(kz_subdevice_output_t *self);
    void (*update)(kz_subdevice_output_t *self);
    uint8_t do_report_flag;
    void (*do_report)(kz_subdevice_output_t *self);
} kz_subdevice_output_vtbl_t;

typedef struct kz_subdevice_output{
    kz_subdevice_output_vtbl_t vtbl;
    kz_device_t* parent_addr; //母设备地址
    uint32_t last_active_timestamp;
    uint8_t update_flag;
    uint8_t report_flag;
} kz_subdevice_output_t;

typedef struct kz_subdevice_status_vtbl{
    void (*update)(kz_subdevice_status_t *self);
} kz_subdevice_status_vtbl_t;

typedef struct kz_subdevice_status{
    kz_subdevice_status_vtbl_t vtbl; //虚函数表
    kz_device_t* parent_addr; //母设备地址
} kz_subdevice_status_t;

typedef struct kz_device_inputbuf{
    uint8_t basickbd_buff[29];
} kz_device_inputbuf_t;

typedef struct kz_device{
    void (*process)(kz_device_t* self);
    kz_subdevice_input_t** input_device; //输入设备数组首地址
    uint8_t input_subdevice_num; //输入设备数量
    kz_subdevice_output_t** output_device; //输出设备首地址
    uint8_t output_subdevice_num; //输出设备数量
    kz_subdevice_status_t** status_device; //输出设备首地址
    uint8_t status_subdevice_num; //输出设备数量
    uint32_t layer_status; //激活层的位掩码
    uint8_t defalut_layer; //默认层
    uint8_t mode;
    kz_device_inputbuf_t inputbuf; // 输入缓存
} kz_device_t;

enum{
    MODE_MASTER,
    MODE_SLAVE,
};

#define KZ_GET_LAYER_STATUS(X,Bitmask) ((Bitmask>>X)&0x01)
#define KZ_SET_LAYER_STATUS(X,Layer) (X|=(0x01<<Layer))
#define KZ_RESET_LAYER_STATUS(X,Layer) (X&=(~(0x01<<Layer)))

// #define KZ_LAYER_BIT(X) ((uint32)1<<X)

#endif
