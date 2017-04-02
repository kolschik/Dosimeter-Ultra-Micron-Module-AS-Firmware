#include <stdio.h>
#include <stdint.h>
#include "STM32L1xx.h"          // Device header

#include "main.h"

ADCDataDef ADCData;
PumpDataDef PumpData;

////////////

uint8_t LED_show_massive[4];    // сырое отображение на дисплее
char LED_BUF[6];
uint32_t counter = 0;
uint32_t SPECTRO_MASSIVE[2049];
FunctionalState Need_Ledupdate = DISABLE;
uint32_t IMPULSE_MASSIVE[11];
uint32_t PUMP_MASSIVE[11];
uint32_t ERR_MASSIVE[11];

uint8_t temperature;
uint16_t akb_voltage;
uint32_t feu_voltage = 650;
uint32_t counter_err = 0;
uint32_t counter_pump = 0;
uint16_t USB_spectro_pointer = 0;


/////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{
  //  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x3000);
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  //set_msi();
  set_pll_for_usb();
  USB_on();

  io_init();


  feu_voltage = eeprom_read(0x00);

  adc_init();
  adc_calibration();

  dac_init();
  dac_on();
  dac_reload();

  PumpCompCmd(INIT_COMP);

  PumpCompCmd(ON_COMP);


  tim2_Config();                // Обслуживание дисплея
  tim3_Config();                // Генерация ВВ
  tim4_Config();                // Учет длительности накачки
  tim9_Config();                // Счет 1 секунды

  EXTI15_Config();

  while (TRUE)
  {
    if(COMP_GetOutputLevel(COMP_Selection_COMP2) == COMP_OutputLevel_Low)
    {
      PumpCmd(ENABLE);
    }

    if(Need_Ledupdate == ENABLE)
    {
      Need_Ledupdate = DISABLE;
      LEDUpdate();
    }
    USB_work();
  }
}
