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
#include "rtc.h"

#include "usb.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "usb_desc.h"

#define C_SEG_ALLOFF 0
#define C_SEG_A (uint16_t)0x0008
#define C_SEG_B (uint16_t)0x0080
#define C_SEG_C (uint16_t)0x0001
#define C_SEG_D (uint16_t)0x0040
#define C_SEG_E (uint16_t)0x0004
#define C_SEG_F (uint16_t)0x0002
#define C_SEG_G (uint16_t)0x1000
#define C_DIG1 (uint16_t) 0x0004
#define C_DIG2 (uint16_t) 0x0400
#define C_DIG3 (uint16_t) 0x0800

#define spectro_data 4

#define INIT_COMP 0
#define ON_COMP 1
#define OFF_COMP 2

#define HI_Z_State 2
#define L_State 0
#define H_State 1
#define Unknown_State 4


#define FLASH_PAGE_SIZE                 0x100   // ( !   USB !!)
#define FLASH_START_ADDR                0x08008000
#define FLASH_END_ADDR                  0x0801FFFF
#define FLASH_MAX_PAGE                  (FLASH_END_ADDR - FLASH_START_ADDR) / FLASH_PAGE_SIZE
#define FLASH_MAX_ELEMENT               FLASH_MAX_PAGE * (FLASH_PAGE_SIZE >> 3)
#define ADC1_DR_ADDRESS    ((uint32_t)0x40012458)


typedef struct
{
  FunctionalState Active;       //   
} PumpDataDef;

typedef struct
{
  uint32_t Batt_voltage_raw;    //
  uint32_t Temp_voltage_raw;    //
  uint32_t Ref_voltage_raw;     //

  uint32_t Temp;

  uint32_t Batt_voltage;        //  
  uint32_t Power_voltage;       //   
  float Calibration_bit_voltage;        //   
  uint32_t DAC_voltage_raw;     //    DAC
} ADCDataDef;

typedef struct
{
  uint32_t feu_voltage;
  uint8_t ADC_bits;
  uint8_t Sound;
  uint8_t LED_intens;
  uint8_t T_korr;
  uint8_t Impulse_dead_time;


} SettingsDef;

typedef struct
{
  FunctionalState USB;
  FunctionalState Charging;
  FunctionalState Spectr;
  FunctionalState Engage;
  FunctionalState Finder;
  FunctionalState Sound;
  FunctionalState Off_mode;
  uint8_t key_hold;

} PowerStateDef;


extern PowerStateDef PowerState;
extern ADCDataDef ADCData;
extern PumpDataDef PumpData;
extern SettingsDef Settings;

extern uint16_t USB_spectro_pointer;
extern uint16_t LED_show_massive[];     // сырое отображение на дисплее
extern char LED_BUF[];
extern uint32_t counter;

extern FunctionalState Need_Ledupdate;
extern FunctionalState Need_pump;
extern FunctionalState PUMP_DEAD_TIME;
extern FunctionalState IMPULSE_DEAD_TIME;
extern FunctionalState debug_mode;
extern FunctionalState sleep_rtc;
extern FunctionalState need_MCP_update;

extern uint32_t SPECTRO_MASSIVE[];
extern uint32_t IMPULSE_MASSIVE[];
extern uint32_t PUMP_MASSIVE[];
extern uint16_t USB_spectro_pointer;
extern uint8_t temperature;
extern uint16_t akb_voltage;
extern uint32_t counter_pump;
extern uint32_t spectro_time;
extern __IO uint16_t ADC_ConvertedValue[];

void LEDString(void);
void LEDUpdate(void);
//void LED_show(uint8_t, uint8_t);

#endif
