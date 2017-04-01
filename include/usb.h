#ifndef __usb_user_H
#define __usb_user_H

#include "stm32l1xx.h"

void USB_on(void);
void USB_off(void);
void USB_work(void);
void USB_send_maxfon_data(void);
void USB_send_doze_data(void);
uint8_t prepare_data(uint32_t massive, uint16_t * massive_pointer, uint8_t start_key);
void eeprom_send(uint32_t current_rcvd_pointer);
#endif
