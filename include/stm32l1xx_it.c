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
void TIM10_IRQHandler(void)     // Обслуживание контроля напряжения
{
  if(TIM_GetITStatus(TIM10, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM10, TIM_IT_Update);


    if(COMP_GetOutputLevel(COMP_Selection_COMP2) == COMP_OutputLevel_Low)
    {
      if(PumpData.Agressive)    // Если накачку надо жестко стабилизировать
        PumpCmd(ENABLE);

      pump_per_second_mass[0]++;        // Статистический учет состояния компаратора
    } else
    {
      if(PumpData.Agressive)    // Если накачку надо жестко стабилизировать
        PumpCmd(DISABLE);

      pump_per_second_mass[0]--;        // Статистический учет состояния компаратора
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t tmpadc1;
void ADC1_IRQHandler(void)
{
  uint16_t address;

  // с момента начала переднего фронта прошло 5.2 мкс
  if(ADC_GetITStatus(ADC1, ADC_IT_JEOC) != RESET)
  {
    ADC_ClearITPendingBit(ADC1, ADC_IT_JEOC);

    // с момента начала переднего фронта прошло 7 мкс
    address = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);

    if((!PumpData.Active) && (!PUMP_DEAD_TIME)) // Если прерывание вызвано не импульсом накачки
    {
      if(address < (Settings.Start_channel << 1))       // Убираем шум в незначемых каналах
      {
        return;
      }


      if(PowerState.Spectr)
      {
        // Режим спектрометра
        SPECTRO_MASSIVE[address >> 1]++;        // Если все нормально, добавляем спектр
        IMPULSE_MASSIVE[0]++;   // увеличиваем счетчик                                
      } else
      {
        GPIOA->BSRRL = GPIO_Pin_4;      //  Вывод сигнала на внешнее устройство - начало фронта
        IMPULSE_MASSIVE[0]++;   // увеличиваем счетчик
        if((tmpadc1 > Settings.Sound) && (Settings.Sound > 0) && PowerState.Sound)
        {
          tmpadc1 = 0;
          TIM4->EGR |= 0x0001;  // Подаем звук
          TIM_CCxCmd(TIM4, TIM_Channel_4, TIM_CCx_Enable);
          TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

        } else
        {
          tmpadc1++;
        }
        GPIOA->BSRRH = GPIO_Pin_4;      //  Вывод сигнала на внешнее устройство - конец фронта
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Накачка
uint32_t tmptim3 = 0, counts;

void TIM3_IRQHandler(void)
{
  uint32_t i;
  if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

    // Проверка, включен ли канал таймера
    i = TIM3->CCER & (uint16_t) (TIM_CCx_Enable << TIM_Channel_2);
    if(i > 0)
      PUMP_DEAD_TIME = ENABLE;  // начинаем отсчет мертвого времени накачки


    if(PumpData.Agressive)      // Если накачку надо жестко стабилизировать
    {

      if(PumpData.Active == DISABLE)    // если накачку надо выключить
      {
        if(tmptim3 > 0)
        {
          tmptim3 = 0;
          //TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
          //TIM_ITConfig(TIM3, TIM_IT_CC2, DISABLE);
          TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Disable);     // запретить накачку
          //PUMP_DEAD_TIME = DISABLE;

        } else
        {
          tmptim3++;
        }
      } else
      {
        tmptim3 = 0;
        if(COMP_GetOutputLevel(COMP_Selection_COMP2) == COMP_OutputLevel_High)
        {
          PumpCmd(DISABLE);
        }
      }
    }
  }

  if(TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
    if(PUMP_DEAD_TIME)
    {
      PUMP_MASSIVE[0]++;
      PUMP_DEAD_TIME = DISABLE; // заканчиваем отсчет мертвого времени накачки
    }
  }
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
int tim9cnt, tim9cnt_spectr_heatup;
void TIM9_IRQHandler(void)
{
  uint32_t i;
  if(TIM_GetITStatus(TIM9, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM9, TIM_IT_Update);

    if(PowerState.Spectr)
      spectro_time++;           // счет времени накопления спектра

    if((!PowerState.USB) && (PowerState.Spectr))        // Автономный сбор спектра
    {
      if(tim9cnt_spectr_heatup < 600)   // Ждем 60 секунд
      {
        tim9cnt_spectr_heatup++;
        if((tim9cnt_spectr_heatup % 10) == 0)   // подаем звук каждую секунду и мигаем светодиодом
        {
          for (i = 0; i < 1000; i++)
          {
            TIM_Cmd(TIM2, DISABLE);     // Индикацию выключить
            TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
            TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

            LED_show(LED_show_massive[0], C_SEG_ALLOFF);
            LED_show(1, C_SEG_A);       // Включаем для индикации 1 сегмент

            TIM_CCxCmd(TIM4, TIM_Channel_4, TIM_CCx_Enable);
          }
          LED_show(LED_show_massive[0], C_SEG_ALLOFF);
          TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);    //выключаем звук
        }
      } else
      {
        if(!PowerState.Engage)
        {
          LED_show(1, C_SEG_A); // Включаем для индикации 1 сегмент
          spectro_time = 0;     // Сброс счета времени накопления спектра
          for (i = 0; i <= 2047; i++)   // Сброс массива спектра
            SPECTRO_MASSIVE[i] = 0;
          PowerState.Engage = ENABLE;   // Запускаем сбор спектра
        }
      }

    }


    if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8))
    {
      PowerState.key_hold++;
    } else
    {
      PowerState.key_hold = 0;
    }

    if(PowerState.key_hold > 20)        // Если держим кнопку 3 секунды - отключение устройства
    {
      PowerState.Off_mode = ENABLE;
    }
    // -----------------------------
    // ротация массивов
    counter = 0;
    counter_pump = 0;
    PumpData.two_sec_sum = 0;

    for (i = 20; i > 0; i--)
    {
      IMPULSE_MASSIVE[i] = IMPULSE_MASSIVE[i - 1];
      PUMP_MASSIVE[i] = PUMP_MASSIVE[i - 1];
      counter += IMPULSE_MASSIVE[i - 1];
      counter_pump += PUMP_MASSIVE[i - 1];

      pump_per_second_mass[i] = pump_per_second_mass[i - 1];
      PumpData.two_sec_sum += pump_per_second_mass[i - 1];      // Сумма за две секунды

    }
    counter >>= 1;              // деление на два
    counter_pump >>= 1;         // деление на два
    IMPULSE_MASSIVE[0] = 0;
    PUMP_MASSIVE[0] = 0;

    pump_per_second_mass[0] = 0;

    // Если требуется значительная компенсация, переключаем в режим жесткой накачки
    if((PumpData.two_sec_sum > 4500) || (PumpData.two_sec_sum < -4500))
    {
      if(PumpData.good_stable_pumps > 100)
        PumpData.good_stable_pumps = 100;

      if(PumpData.good_stable_pumps < 70)
      {
        PumpData.good_stable_pumps = 0;
        if(!PumpData.Agressive)
        {
          PumpData.Agressive = ENABLE;
          PumpCompCmd(INIT_COMP);
        }
      } else
      {
        PumpData.good_stable_pumps--;
      }
    } else
    {
      PumpData.good_stable_pumps++;
      if(PumpData.good_stable_pumps > 100)      // 10 секунд накачка стабильна
      {
        PumpCompCmd(NVIC_DEINIT);
        PumpData.Average_pump = counter_pump;
        counts = counter_pump;
        PumpData.Agressive = DISABLE;
        TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Enable);        // разрешить накачку   
      }
    }
    // -----------------------------

    if(!PumpData.Agressive)     // Если накачку надо нежно стабилизировать
    {
      pump_per_second = counts + ((pump_per_second_mass[1] + pump_per_second_mass[2] + pump_per_second_mass[3]) / 50);


      // Удержание накачки в окне стабилизации (+-100 имп/c, не не менее 50-ти импульсов)
      if(PumpData.Average_pump > 150)
      {
        if(pump_per_second < (PumpData.Average_pump - 100))
          pump_per_second = PumpData.Average_pump - 100;
      } else
      {
        if(pump_per_second < 50)
          pump_per_second = 50;
      }

      if(pump_per_second > (PumpData.Average_pump + 100))
        pump_per_second = PumpData.Average_pump + 100;
      // -----------------------------

      // Установка нового значения таймера накачки
      i = timer_freq / (pump_per_second);
      if(i > 0xFFFF)
        i = 0xFFFF;

      TIM_SetAutoreload(TIM3, i);
      counts = counter_pump;
      // -----------------------------
    }
    // -----------------------------


    if(tim9cnt >= 25)           // Каждые 2.5 секунды
    {
      tim9cnt = 0;
      need_MCP_update = ENABLE; // надо проверить USB

      if(PowerState.USB)
        LED_show(LED_show_massive[0], C_SEG_ALLOFF);

      if(akb_voltage < 340)     // меньше 3.4 вольта, выключить устройство!
      {
        if(!PowerState.Low_bat)
        {
          PowerState.Low_bat = ENABLE;
        } else
        {
          // перезагрузка устройства для входа в сон
          RTC_WriteBackupRegister(RTC_BKP_DR0, 0x1212);
          IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
          IWDG_SetPrescaler(IWDG_Prescaler_4);
          IWDG_SetReload(2);
          IWDG_ReloadCounter();
          IWDG_Enable();
          while (1);
        }
      }
    } else
      tim9cnt++;



    if(!PowerState.Low_bat)
    {
      if(counter > 999)
      {
        sprintf(LED_BUF, "%3u", counter / 1000);        // Пишем в буфер значение счетчика
      } else
      {
        sprintf(LED_BUF, "%3u", counter);       // Пишем в буфер значение счетчика
      }
    } else
    {
      sprintf(LED_BUF, "Lo ");
    }

    LEDString();                // // Выводим обычным текстом содержание буфера
    // проверяем напряжение         
    if(PumpData.Agressive)      // Если накачку надо жестко стабилизировать
      if(COMP_GetOutputLevel(COMP_Selection_COMP2) == COMP_OutputLevel_High)
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
    // Вычисление напряжения АКБ
    akb_voltage = (float) (ADCData.Calibration_bit_voltage * ADCData.Batt_voltage_raw * 100 * 2);


    ts_cal1 = *((uint16_t *) ((uint32_t) 0x1FF8007A));
    ts_cal2 = *((uint16_t *) ((uint32_t) 0x1FF8007E));
    Vrefint_cal = *((uint16_t *) ((uint32_t) 0x1FF80078));

    Vdda = 3 * ((float) Vrefint_cal / ADCData.Ref_voltage_raw);
    TS_data_norm = ADCData.Temp_voltage_raw * Vdda / 3;
    temperature_norm = (((float) (110 - 30) / (ts_cal2 - ts_cal1)) * (TS_data_norm - ts_cal1) + 30);    //с нормализацией

    ADCData.Temp = temperature_norm;

  }
}

///////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////
// =======================================================
// Прерывание по кнопке
void EXTI9_5_IRQHandler(void)
{
  uint32_t i = 0, jitter = 0;
  if(EXTI_GetITStatus(EXTI_Line8) != RESET)
  {
    if(!sleep_rtc)
    {
      LED_show(LED_show_massive[0], C_SEG_ALLOFF);
      for (i = 0; i < 100000; i++)      // устранение дребезга кнопки
      {
        while (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8))
        {
          if(jitter > 60000)
            TIM_CCxCmd(TIM4, TIM_Channel_4, TIM_CCx_Enable);    // Если кнопка зажата, издаем звук

          jitter++;
        }
      }
      TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);        //если отжата, выключаем звук

      if(jitter > 60000)
      {
        if(PowerState.USB)
        {
          // Если влключен USB режим, выключить индикацию, включить спектрометр
          PowerState.Spectr = ENABLE;
          dac_on();             // Включение ЦАП
          PumpCompCmd(INIT_COMP);       // Включение компоратора
          PumpCompCmd(ON_COMP);
          TIM_Cmd(TIM10, ENABLE);       // Включение контроля напряжения на ФЭУ

          TIM_Cmd(TIM2, DISABLE);       // Индикацию выключить
          TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
          TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
          LED_show(LED_show_massive[0], C_SEG_ALLOFF);

          PowerState.Sound = DISABLE;   // Звук выключить
        } else
        {
          // Если вылключен USB режим, а спектрометр не активен, включить спектрометр
          if(!PowerState.Spectr)
          {
            PowerState.Spectr = ENABLE;
            tim9cnt_spectr_heatup = 0;
            PowerState.Engage = DISABLE;
            dac_on();           // Включение ЦАП
            PumpCompCmd(INIT_COMP);     // Включение компоратора
            PumpCompCmd(ON_COMP);
            TIM_Cmd(TIM10, ENABLE);     // Включение контроля напряжения на ФЭУ

            TIM_Cmd(TIM2, DISABLE);     // Индикацию выключить
            TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
            TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
            LED_show(LED_show_massive[0], C_SEG_ALLOFF);
            LED_show(1, C_SEG_A);       // Включаем для индикации 1 сегмент

            PowerState.Sound = DISABLE; // Звук выключить
          } else
            // Если вылключен USB режим, и спектрометр активен, перейти в режим поиска
          {
            PowerState.Spectr = DISABLE;
            dac_on();           // Включение ЦАП
            PumpCompCmd(INIT_COMP);     // Включение компоратора
            PumpCompCmd(ON_COMP);
            TIM_Cmd(TIM10, ENABLE);     // Включение контроля напряжения на ФЭУ

            TIM_Cmd(TIM2, ENABLE);      // Индикацию включить
            TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
            TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
            LED_show(LED_show_massive[0], C_SEG_ALLOFF);

            PowerState.Sound = ENABLE;  // Звук включить

          }
        }
      }
    }

    EXTI_ClearITPendingBit(EXTI_Line8);
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
