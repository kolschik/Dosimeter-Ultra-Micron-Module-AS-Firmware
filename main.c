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
uint32_t spectro_time = 0;
uint16_t USB_spectro_pointer = 0;
FunctionalState debug_mode = DISABLE;
__IO uint16_t ADC_ConvertedValue[384];

/////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{
  //  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x3000);
//  DBGMCU_Config(DBGMCU_SLEEP | DBGMCU_STANDBY | DBGMCU_STOP, ENABLE);
  Power_on();

  while (TRUE)
  {

    if(PowerState.Charging && !PowerState.USB)  // Если питание подано, а USB еще не включен, включаем!
    {
      USB_on();
      PowerState.Spectr = DISABLE;
      dac_off();                // Выключение ЦАП
      PumpCompCmd(OFF_COMP);    // Выключение компоратора
      TIM_Cmd(TIM10, DISABLE);  // Выключение контроля напряжения на ФЭУ
      PumpCmd(DISABLE);

      TIM_Cmd(TIM2, DISABLE);   // Индикацию выключить
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
    //PWR_EnterSleepMode(PWR_Regulator_ON, PWR_SLEEPEntry_WFI);
    if(PowerState.Off_mode)
    {
      PWR_FastWakeUpCmd(ENABLE);
      PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
      PWR_FastWakeUpCmd(DISABLE);
    }

  }
}
