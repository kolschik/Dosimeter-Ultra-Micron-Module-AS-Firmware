#include <stdio.h>
#include <stdint.h>
#include "STM32L1xx.h"          // Device header

#include "main.h"

ADCDataDef ADCData;
PumpDataDef PumpData;
SettingsDef Settings;
PowerStateDef PowerState;

////////////

uint16_t LED_show_massive[4];   // сырое отображение на дисплее
char LED_BUF[6];
uint32_t counter = 0;
uint32_t SPECTRO_MASSIVE[2049];
uint32_t IMPULSE_MASSIVE[21];
uint32_t PUMP_MASSIVE[21];

FunctionalState Need_Ledupdate = DISABLE;
FunctionalState Need_pump = DISABLE;
FunctionalState PUMP_DEAD_TIME = DISABLE;
FunctionalState IMPULSE_DEAD_TIME = DISABLE;

uint8_t temperature;
uint16_t akb_voltage;
uint32_t counter_pump = 0;
uint32_t spectro_time = 0;
uint16_t USB_spectro_pointer = 0;
FunctionalState debug_mode = DISABLE;
__IO uint16_t ADC_ConvertedValue[384];
FunctionalState sleep_rtc = DISABLE;
FunctionalState need_MCP_update = DISABLE;

uint32_t pump_per_second = 2700;
int pump_per_second_mass[23];


/////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{
  uint32_t chkkeysleep = 0;     //, eeprom_datamassive = 0;

  RTC_Config();
  RTC_WriteBackupRegister(RTC_BKP_DR0, 0x0000);
  if(sleep_rtc)                 // Проверка необходимости ухода в сон.
  {
    io_init();
    EXTI8_Config();             // Кнопка

    Power_off();

    while (chkkeysleep < 200000)
    {
      chkkeysleep = 0;
      PWR_FastWakeUpCmd(ENABLE);
      PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
      PWR_FastWakeUpCmd(DISABLE);

      while (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8)) // проверка зажатой кнопки
        chkkeysleep++;
    }

    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_4);
    IWDG_SetReload(2);
    IWDG_ReloadCounter();
    IWDG_Enable();
    while (1);
  }

  Power_on();

  PowerState.Sound = ENABLE;    // Звук включить


  while (TRUE)
  {

    if(PowerState.Charging && !PowerState.USB)  // Если питание подано, а USB еще не включен, включаем!
    {
      USB_on();
      PowerState.Spectr = ENABLE;
      dac_on();                 // Включение ЦАП
      PumpCompCmd(INIT_COMP);   // Включение компоратора
      PumpCompCmd(ON_COMP);
      TIM_Cmd(TIM10, ENABLE);   // Включение контроля напряжения на ФЭУ

      TIM_Cmd(TIM2, DISABLE);   // Индикацию выключить
      TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
      TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
      LED_show(LED_show_massive[0], C_SEG_ALLOFF);
      PowerState.Sound = DISABLE;       // Звук выключить

    }

    if(!PowerState.Charging && PowerState.USB)  // Если питание снято, а USB еще включен, выключаем!
    {
      USB_off();
    }


    if(Need_Ledupdate == ENABLE)
    {
      Need_Ledupdate = DISABLE;
      LEDUpdate();
    }

    if(PowerState.USB)
    {
      USB_work();
    }

    if(need_MCP_update)
    {
      need_MCP_update = DISABLE;
      switch (MCP73831_state_detect())
      {
      case HI_Z_State:
        PowerState.Charging = DISABLE;
        break;

      case L_State:
        PowerState.Charging = ENABLE;
        break;

      case H_State:
        PowerState.Charging = ENABLE;
        break;
      }

    }

    if(PowerState.Off_mode)     // Проверка необходимости ухода в сон.
    {
      TIM_Cmd(TIM2, DISABLE);   // Индикацию выключить
      TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
      TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

      LED_show(LED_show_massive[0], C_SEG_ALLOFF);

      LED_show(1, C_SEG_A);     // Включаем для индикации 1 сегмент
      delay_ms(100);
      LED_show(1, C_SEG_B);     // Включаем для индикации 1 сегмент
      delay_ms(100);
      LED_show(1, C_SEG_C);     // Включаем для индикации 1 сегмент
      delay_ms(100);
      LED_show(1, C_SEG_D);     // Включаем для индикации 1 сегмент
      delay_ms(100);
      LED_show(1, C_SEG_E);     // Включаем для индикации 1 сегмент
      delay_ms(100);
      LED_show(1, C_SEG_F);     // Включаем для индикации 1 сегмент
      delay_ms(100);
      LED_show(1, C_SEG_G);     // Включаем для индикации 1 сегмент
      delay_ms(100);

      Power_off();

      // перезагрузка устройства для входа в сон
      RTC_WriteBackupRegister(RTC_BKP_DR0, 0x1212);
      IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
      IWDG_SetPrescaler(IWDG_Prescaler_4);
      IWDG_SetReload(2);
      IWDG_ReloadCounter();
      IWDG_Enable();
      while (1);
    }

    if((PowerState.Engage) && (!PowerState.Spectr))     // Если был осуществлен набор спектра, сохраняем спектр
    {
      Power_off();

      eeprom_write((0x100 + 0x04), spectro_time);       // запись времени спектра
      eeprom_write((0x100 + 0x08), ADCData.Temp & 0xff);        // запись температуры спектра
      full_erase_flash();       // очистка данных FLASH
      flash_write_massive(0);

      PowerState.Engage = DISABLE;
      IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
      IWDG_SetPrescaler(IWDG_Prescaler_4);
      IWDG_SetReload(2);
      IWDG_ReloadCounter();
      IWDG_Enable();
      while (1);
    }
  }
}
