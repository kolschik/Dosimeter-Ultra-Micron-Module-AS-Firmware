#include <stdio.h>
#include <stdint.h>
#include "STM32L1xx.h"          // Device header

#include "main.h"

ADCDataDef ADCData;
PumpDataDef PumpData;
SettingsDef Settings;

////////////

uint8_t LED_show_massive[4];    // сырое отображение на дисплее
char LED_BUF[6];
uint32_t counter = 0;
uint32_t SPECTRO_MASSIVE[2049];
uint32_t IMPULSE_MASSIVE[11];
uint32_t PUMP_MASSIVE[11];
uint32_t ERR_MASSIVE[11];

FunctionalState Need_Ledupdate = DISABLE;
FunctionalState Need_pump = DISABLE;
FunctionalState PUMP_DEAD_TIME = DISABLE;
FunctionalState IMPULSE_DEAD_TIME = DISABLE;

uint8_t temperature;
uint16_t akb_voltage;
uint32_t counter_err = 0;
uint32_t counter_pump = 0;
uint16_t USB_spectro_pointer = 0;
FunctionalState debug_mode = DISABLE;
__IO uint16_t ADC_ConvertedValue[384];

/////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{
  //  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x3000);

  set_pll_for_usb();
  delay_ms(300);
  DBGMCU_Config(DBGMCU_SLEEP | DBGMCU_STANDBY | DBGMCU_STOP, ENABLE);

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  //set_msi();

  USB_on();

  io_init();


  Settings.feu_voltage = eeprom_read(0x10);
  Settings.ADC_bits = eeprom_read(0x14);
  Settings.Sound = eeprom_read(0x18);
  Settings.LED_intens = eeprom_read(0x1C);
  Settings.T_korr = eeprom_read(0x20);
  Settings.Impulse_dead_time = eeprom_read(0x28);


  adc_init();

  dac_init();
  dac_on();

  PumpCompCmd(INIT_COMP);

  PumpCompCmd(ON_COMP);


  tim2_Config();                // Обслуживание дисплея
  tim3_Config();                // Генерация ВВ
  tim4_Config();                // обслуживание звука
  tim9_Config();                // Счет 1 секунды
  tim10_Config();               // Учет мертвого времени для измерений

  EXTI15_Config();

  while (TRUE)
  {
    if(COMP_GetOutputLevel(COMP_Selection_COMP2) == COMP_OutputLevel_Low)
    {
      Need_pump = ENABLE;
    } else
    {
      PumpCmd(DISABLE);
    }

    if(Need_pump == ENABLE)
      if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15))
      {
        Need_pump = DISABLE;
        PumpCmd(ENABLE);
      }

    if(Need_Ledupdate == ENABLE)
    {
      Need_Ledupdate = DISABLE;
      LEDUpdate();
    }
    USB_work();
    //  PWR_EnterSleepMode(PWR_Regulator_ON, PWR_SLEEPEntry_WFI);
  }
}
