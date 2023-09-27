#ifndef _USB_HANDLER_H
#define _USB_HANDLER_H

#include <stdint.h>
#include <string.h>


void usb_init();
void read_to_usb_temp_6key(uint8_t buf_6key[8]);
void usb_process();
void generate_kbd_8bit_keycode(uint8_t buff_29key[29],uint8_t buff_8key[8]);
uint8_t is_usb_avaliable();

#endif
