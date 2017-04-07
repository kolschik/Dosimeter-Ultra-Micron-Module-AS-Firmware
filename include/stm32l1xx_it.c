#include "stm32l1xx_it.h"
#include "main.h"
#include <stdio.h>

// ===============================================================================================
void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  while (1)
  {
  }
}

/**
* @brief  This function handles Memory Manage exception.
* @param  None
* @retval None
*/
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
* @brief  This function handles Bus Fault exception.
* @param  None
* @retval None
*/
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
* @brief  This function handles Usage Fault exception.
* @param  None
* @retval None
*/
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
* @brief  This function handles SVCall exception.
* @param  None
* @retval None
*/
void SVC_Handler(void)
{
}

/**
* @brief  This function handles Debug Monitor exception.
* @param  None
* @retval None
*/
void DebugMon_Handler(void)
{
}

/**
* @brief  This function handles PendSVC exception.
* @param  None
* @retval None
*/
void PendSV_Handler(void)
{
}

/**
* @brief  This function handles SysTick Handler.
* @param  None
* @retval None
*/
void SysTick_Handler(void)
{
  extern uint32_t msTicks;
  msTicks++;                    // инкремент счётчика времени
}

/******************************************************************************/
/*                 STM32L1xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l1xx_xx.s).                                            */
/******************************************************************************/

/**
* @brief  This function handles PPP interrupt request.
* @param  None
* @retval None
*/
/*void PPP_IRQHandler(void)
{
}*/

/**
* @}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TIM2_IRQHandler(void)      // Обновление дисплея
{
  if(TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);

    LED_show(LED_show_massive[0], C_SEG_ALLOFF);        // отключаем все сигменты
  }


  if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    Need_Ledupdate = ENABLE;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TIM10_IRQHandler(void)     // Учет мертвого времени датчика
{
  if(TIM_GetITStatus(TIM10, TIM_IT_CC1) != RESET)
  {
    TIM_ClearITPendingBit(TIM10, TIM_IT_CC1);

    if(debug_mode)
      GPIO_ResetBits(GPIOA, GPIO_Pin_4);        // Вывод сигнала на внешнее устройство

    IMPULSE_DEAD_TIME = DISABLE;
    TIM_ITConfig(TIM10, TIM_IT_CC1, DISABLE);
  }

  if(TIM_GetITStatus(TIM10, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM10, TIM_IT_Update);

    TIM_ITConfig(TIM10, TIM_IT_Update, DISABLE);

  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t tmpadc1;
void ADC1_IRQHandler(void)
{
  uint16_t address;
  if(ADC_GetITStatus(ADC1, ADC_IT_JEOC) != RESET)
  {
    ADC_ClearITPendingBit(ADC1, ADC_IT_JEOC);

    if(!PUMP_DEAD_TIME)         // Если прерывание вызвано не импульсом накачки
    {
      address = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);

      if(!debug_mode)
        GPIO_SetBits(GPIOA, GPIO_Pin_4);        //  Вывод сигнала на внешнее устройство

//      if(!IMPULSE_DEAD_TIME)    // Проверка что прошлый импульс с датчика был завершен
      {
        if(Settings.ADC_bits > 0)
        {
          SPECTRO_MASSIVE[address >> 1]++;      // Если все нормально, добавляем спектр
        } else
        {
          SPECTRO_MASSIVE[(address >> 2) << 1]++;       // Если все нормально, добавляем спектр
          SPECTRO_MASSIVE[((address >> 2) << 1) + 1]++; // Если все нормально, добавляем спектр
        }


//        if(debug_mode)
//          GPIO_SetBits(GPIOA, GPIO_Pin_4);      //  Вывод сигнала на внешнее устройство

//        IMPULSE_DEAD_TIME = ENABLE;     // начинаем отсчет мертвого времени датчика
//        TIM_ClearITPendingBit(TIM10, TIM_IT_CC1);
//        TIM_ITConfig(TIM10, TIM_IT_CC1, ENABLE);
//        TIM10->EGR |= 0x0001;

      }
//                      else
//      {
//        ERR_MASSIVE[0]++;       // Подсчитываем ошибки снятия спектра
//      }


      if(COMP_GetOutputLevel(COMP_Selection_COMP2) == COMP_OutputLevel_Low)
      {
        PumpCmd(ENABLE);
      }

      IMPULSE_MASSIVE[0]++;     // увеличиваем счетчик

      if((tmpadc1 > Settings.Sound) && (Settings.Sound > 0))
      {
        tmpadc1 = 0;
        TIM4->EGR |= 0x0001;    // Подаем звук
        TIM_CCxCmd(TIM4, TIM_Channel_4, TIM_CCx_Enable);
        TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

      } else
        tmpadc1++;
      if(!debug_mode)
        GPIO_ResetBits(GPIOA, GPIO_Pin_4);      //  Вывод сигнала на внешнее устройство
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Накачка
int tmptim3;
void TIM3_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    if(tmptim3 > 1)
    {
      if(PumpData.Active == DISABLE)    // если накачку надо выключить
      {
        tmptim3 = 0;
        TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
        TIM_ITConfig(TIM3, TIM_IT_CC2, DISABLE);
        PUMP_DEAD_TIME = DISABLE;
      }
    } else
      tmptim3++;

  }

  if(TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);

//    PUMP_DEAD_TIME = ENABLE;    // начинаем отсчет мертвого времени накачки
//    TIM10->EGR |= 0x0001;
//    TIM_ITConfig(TIM10, TIM_IT_CC1, ENABLE);
  }

  PUMP_MASSIVE[0]++;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Звук
int tmptim4;
void TIM4_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

    if(tmptim4 > 2)
    {
      tmptim4 = 0;
      TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);
      TIM_CCxCmd(TIM4, TIM_Channel_4, TIM_CCx_Disable);

    } else
      tmptim4++;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 0.1 секунда
void TIM9_IRQHandler(void)
{
  int i;
  if(TIM_GetITStatus(TIM9, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM9, TIM_IT_Update);

    spectro_time++;             // счет времени накопления спектра

    // -----------------------------
    // ротация массивов
    counter = 0;
    counter_err = 0;
    counter_pump = 0;
    for (i = 10; i > 0; i--)
    {
      IMPULSE_MASSIVE[i] = IMPULSE_MASSIVE[i - 1];
      PUMP_MASSIVE[i] = PUMP_MASSIVE[i - 1];
      ERR_MASSIVE[i] = ERR_MASSIVE[i - 1];
      counter += IMPULSE_MASSIVE[i - 1];
      counter_err += ERR_MASSIVE[i - 1];
      counter_pump += PUMP_MASSIVE[i - 1];
    }
    IMPULSE_MASSIVE[0] = 0;
    PUMP_MASSIVE[0] = 0;
    ERR_MASSIVE[0] = 0;
    // -----------------------------

    if(counter > 999)
    {
      sprintf(LED_BUF, "%3u", counter / 1000);  // Пишем в буфер значение счетчика
    } else
    {
      sprintf(LED_BUF, "%3u", counter); // Пишем в буфер значение счетчика
    }
    LEDString();                // // Выводим обычным текстом содержание буфера

    // проверяем напряжение         
    if(COMP_GetOutputLevel(COMP_Selection_COMP2) == COMP_OutputLevel_Low)
    {
      Need_pump = ENABLE;
    } else
    {
      PumpCmd(DISABLE);
    }


  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Обработка регулярных каналов АЦП
void DMA1_Channel1_IRQHandler(void)
{
  int i;
  uint16_t ts_cal1, ts_cal2, TS_data_norm, Vrefint_cal;
  float Vdda, temperature_norm, tmp;

  if(DMA_GetITStatus(DMA1_IT_TC1))
  {
    DMA_ClearITPendingBit(DMA1_IT_GL1);

    // Усреднение 128-х замеров 
    ADCData.Batt_voltage_raw = 0;
    ADCData.Temp_voltage_raw = 0;
    ADCData.Ref_voltage_raw = 0;
    for (i = 0; i < 384; i += 3)
    {
      ADCData.Ref_voltage_raw += ADC_ConvertedValue[i];
      ADCData.Temp_voltage_raw += ADC_ConvertedValue[i + 1];
      ADCData.Batt_voltage_raw += ADC_ConvertedValue[i + 2];
    }
    ADCData.Ref_voltage_raw >>= 7;
    ADCData.Temp_voltage_raw >>= 7;
    ADCData.Batt_voltage_raw >>= 7;

    tmp = 1.224 / (float) ADCData.Ref_voltage_raw;      // битовое значение соотв. напряжению референса 1.22в, из него вычисляем скольким микровольтам соответствует 1 бит.
    // 0,0007326В на 1 бит
    if(ADCData.Calibration_bit_voltage != tmp)  // Если бит калибровки изменился, перегрузить ЦАП
    {
      ADCData.Calibration_bit_voltage = tmp;
      dac_reload();
    }
    ts_cal1 = *((uint16_t *) ((uint32_t) 0x1FF8007A));
    ts_cal2 = *((uint16_t *) ((uint32_t) 0x1FF8007E));
    Vrefint_cal = *((uint16_t *) ((uint32_t) 0x1FF80078));

    Vdda = 3 * ((float) Vrefint_cal / ADCData.Ref_voltage_raw);
    TS_data_norm = ADCData.Temp_voltage_raw * Vdda / 3;
    temperature_norm = (((float) (110 - 30) / (ts_cal2 - ts_cal1)) * (TS_data_norm - ts_cal1) + 30);    //с нормализацией

    ADCData.Temp = temperature_norm;

  }
}

/*******************************************************************************
* Function Name  : USB_IRQHandler
* Description    : This function handles USB Low Priority interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_LP_IRQHandler(void)
{
  USB_Istr();
}

/*******************************************************************************
* Function Name  : USB_FS_WKUP_IRQHandler
* Description    : This function handles USB WakeUp interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USB_FS_WKUP_IRQHandler(void)
{
  EXTI_ClearITPendingBit(EXTI_Line18);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
