#ifndef __main_user_H
#define __main_user_H

#include "stm32l1xx.h"

#include "adc.h"
#include "clock.h"
#include "dac.h"
#include "delay.h"
#include "eeprom.h"
#include "flash_save.h"
#include "io_ports.h"
#include "interrupt.h"
#include "services.h"
#include "stm32_it.h"
#include "timers.h"
#include "pump.h"

#include "usb.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "usb_desc.h"

#define C_SEG_ALLOFF 0
#define C_SEG_A 1
#define C_SEG_B 2
#define C_SEG_C 3
#define C_SEG_D 4
#define C_SEG_E 5
#define C_SEG_F 6
#define C_SEG_G 7
#define C_DIG1 8
#define C_DIG2 9
#define C_DIG3 10

#define spectro_data 4

#define INIT_COMP 0
#define ON_COMP 1
#define OFF_COMP 2

#define FLASH_PAGE_SIZE                 0x100   // ( !   USB !!)
#define FLASH_START_ADDR                0x0800F000
#define FLASH_END_ADDR                  0x0801FFFF
#define FLASH_MAX_PAGE                  (FLASH_END_ADDR - FLASH_START_ADDR) / FLASH_PAGE_SIZE
#define FLASH_MAX_ELEMENT               FLASH_MAX_PAGE * (FLASH_PAGE_SIZE >> 3)


typedef struct
{
  FunctionalState Active;       //   
  FunctionalState Impulse_past; //   
} PumpDataDef;

typedef struct
{
  uint32_t Batt_voltage_raw;    //  
  uint32_t Batt_voltage;        //  
  uint32_t Power_voltage;       //   
  uint32_t Calibration_bit_voltage;     //   
  uint32_t Procent_battery;     //   
  uint32_t DAC_voltage_raw;     //    DAC
} ADCDataDef;


extern ADCDataDef ADCData;
extern PumpDataDef PumpData;

extern uint16_t USB_spectro_pointer;
extern uint8_t LED_show_massive[];      // сырое отображение на дисплее
extern char LED_BUF[];
extern uint32_t counter;
extern FunctionalState Need_Ledupdate;
extern uint32_t SPECTRO_MASSIVE[];
extern uint32_t IMPULSE_MASSIVE[];
extern uint32_t PUMP_MASSIVE[];
extern uint32_t ERR_MASSIVE[];
extern uint16_t USB_spectro_pointer;
extern uint8_t temperature;
extern uint16_t akb_voltage;
extern uint32_t feu_voltage;
extern uint32_t counter_err;
extern uint32_t counter_pump;

void LEDString(void);
void LEDUpdate(void);
void LED_show(uint8_t, uint8_t, FunctionalState);

#endif
