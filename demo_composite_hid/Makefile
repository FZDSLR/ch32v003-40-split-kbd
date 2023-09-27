all : flash

TARGET:=demo_composite_hid
CH32V003FUN:=../rv003usb/ch32v003fun/ch32v003fun
MINICHLINK?=../rv003usb/ch32v003fun/minichlink

ADDITIONAL_C_FILES+=../rv003usb/rv003usb/rv003usb.S ../rv003usb/rv003usb/rv003usb.c ./hardware/uart.c ./hardware/systick.c ./hardware/ws2812b_data.c ./hardware/usb_handler.c ./RingBuffer/ring_buffer.c
EXTRA_CFLAGS:=-I../rv003usb/lib -I../rv003usb/rv003usb -I./hardware -I./kz -I./RingBuffer

include ../rv003usb/ch32v003fun/ch32v003fun/ch32v003fun.mk

compiledb :
	compiledb -n make

win_cv_clean :
	del $(TARGET).elf $(TARGET).bin $(TARGET).hex $(TARGET).lst $(TARGET).map $(TARGET).hex

flash : cv_flash
clean : cv_clean
win_clean : win_cv_clean
