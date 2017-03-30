#include "stm32l1xx_it.h"
#include "main.h"
#include <stdio.h>

//extern __IO uint32_t CaptureNumber, PeriodValue;
//uint32_t IC1ReadValue1 = 0, IC1ReadValue2 = 0;

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


void TIM2_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);

    LED_show(LED_show_massive[0], C_SEG_ALLOFF, DISABLE);       // отключаем все сигменты
  }


  if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    Need_Ledupdate = ENABLE;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ADC1_IRQHandler(void)
{
  uint16_t address;
  if(ADC_GetITStatus(ADC1, ADC_IT_JEOC) != RESET)
  {
    ADC_ClearITPendingBit(ADC1, ADC_IT_JEOC);

    if(PumpData.Impulse_past == DISABLE)        // Если прерывание вызвано не импульсом накачки
    {
      address = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);

      if(COMP_GetOutputLevel(COMP_Selection_COMP2) == COMP_OutputLevel_Low)
      {
        PumpCmd(ENABLE);
      }


      SPECTRO_MASSIVE[address >> 6]++;
      IMPULSE_MASSIVE[0]++;     // увеличиваем счетчик

      TIM4->EGR |= 0x0001;      // Подаем звук
      TIM_CCxCmd(TIM4, TIM_Channel_4, TIM_CCx_Enable);
      TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

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
        PumpData.Impulse_past = DISABLE;        // сообщаем что импульс прошел.
        TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
      }
    } else
      tmptim3++;

  }

  if(TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
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

    if(tmptim4 > 1)
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

    PumpData.Impulse_past = DISABLE;

    counter = 0;
    for (i = 10; i > 0; i--)
    {
      IMPULSE_MASSIVE[i] = IMPULSE_MASSIVE[i - 1];
      counter += IMPULSE_MASSIVE[i - 1];
    }
    IMPULSE_MASSIVE[0] = 0;

    if(counter > 999)
    {
      counter /= 1000;
    }

    sprintf(LED_BUF, "%3u", counter);   // Пишем в буфер значение счетчика
    LEDString();                // // Выводим обычным текстом содержание буфера
  }
}

/*
// =======================================================
// Реакция на детектор фронта
void EXTI15_10_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line15) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line15);
    if(PumpData.Impulse_past == DISABLE)        // Если прерывание вызвано не импульсом накачки
    {
      IMPULSE_MASSIVE[0]++;     // увеличиваем счетчик

      TIM4->EGR |= 0x0001;      // Подаем звук
      TIM_CCxCmd(TIM4, TIM_Channel_4, TIM_CCx_Enable);  // разрешить накачку   
      TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    }
  }
}
*/















/*
// =======================================================
// Прерывание по нажатию кнопки 0
void EXTI3_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line3) != RESET)
  {
    if(!poweroff_state)
    {
      if(Alarm.Alarm_active && !Alarm.User_cancel)
      {
        Alarm.User_cancel = ENABLE;
        sound_deactivate();
      } else
      {
        if(Power.Display_active)
        {
          key |= 0x1;           // Кнопка Меnu
        }
      }
      Sound_key_pressed = ENABLE;
      Alarm.Tick_beep_count = 0;
      check_wakeup_keys();
    }
    EXTI_ClearITPendingBit(EXTI_Line3);
  }
}

// =======================================================
// Прерывание по нажатию кнопки 1
void EXTI4_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line4) != RESET)
  {
    if(!poweroff_state)
    {
      if(Power.Display_active)
      {
        key |= 0x4;             // Кнопка -
      }
      Sound_key_pressed = ENABLE;
      Alarm.Tick_beep_count = 0;
      check_wakeup_keys();
    }
    EXTI_ClearITPendingBit(EXTI_Line4);
  }
}


// =======================================================
// Прерывание по импульсу от датчика и кнопки 2
void EXTI9_5_IRQHandler(void)
{
  // extern __IO uint8_t Receive_Buffer[64];
//  extern __IO uint32_t Receive_length;
//  extern __IO uint32_t length;
  if(EXTI_GetITStatus(EXTI_Line8) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line8);
    if(!poweroff_state)
    {
      if(Settings.AB_mode < 2)
      {
        Detector_massive[Detector_massive_pointer]++;   // Добавляем пойманную частицу к счетчику  
        ram_Doze_massive[0]++;  // Увеличение суточного массива дозы
      } else
      {
        Detector_AB_massive[0]++;
      }

      if(Settings.Sound && !(Alarm.Alarm_active && !Alarm.User_cancel))
      {
        if(Power.Display_active)
        {
          if(Settings.AMODUL_mode == 0)
            sound_activate();
        }
      }
    }
  }

  if(EXTI_GetITStatus(EXTI_Line6) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line6);
    if(!poweroff_state)
    {
      if(Power.Display_active)
      {
        key |= 0x2;             // Кнопка +
      }
      Sound_key_pressed = ENABLE;
      Alarm.Tick_beep_count = 0;
      check_wakeup_keys();
    }
  }
  if(EXTI_GetITStatus(EXTI_Line9) != RESET)     // Подключено USB
  {
    EXTI_ClearITPendingBit(EXTI_Line9);
    if(!poweroff_state)
    {
      sound_activate();
      Power.sleep_time = Settings.Sleep_time;
    }
  }

}


// ========================================================
// генерация звука на динамик
void TIM3_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    if(!poweroff_state)
    {
      if(Alarm.Alarm_active && !Alarm.User_cancel)
      {
        Alarm.Alarm_beep_count++;
        if(Alarm.Alarm_beep_count == 100)
          TIM_SetAutoreload(TIM10, 32);
        if(Alarm.Alarm_beep_count == 200)
        {
          TIM_SetAutoreload(TIM10, 16);
          Alarm.Alarm_beep_count = 0;
        }
      }

      if((Alarm.Alarm_active && Alarm.User_cancel) || !Alarm.Alarm_active)
      {
        if(Power.Sound_active == ENABLE)
        {
          if(Sound_key_pressed) // нажатие кнопки
          {
            if(Alarm.Tick_beep_count > 80)
            {
              Alarm.Tick_beep_count = 0;
              sound_deactivate();
            } else
              Alarm.Tick_beep_count++;


          } else if(((Alarm.Tick_beep_count > 6) && (Settings.AMODUL_mode == 0)) || ((Alarm.Tick_beep_count > 1) && (Settings.AMODUL_mode != 0)))       // тик датчика или тик модуля
          {
            Alarm.Tick_beep_count = 0;
            sound_deactivate();
          } else
            Alarm.Tick_beep_count++;
        } else
          sound_deactivate();
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Модуль-А

void TIM2_IRQHandler(void)
{
  uint32_t i = 0;
  if(TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);

    if(Settings.AMODUL_mode == 0)       // Если модуль-А выключен, активируем его
      plus_amodul_engage(0x00);

    if(spect_impulse == DISABLE)
    {
      if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1))
      {
        spect_impulse = ENABLE;
        Data.AMODULE_timstart = TIM_GetCapture2(TIM2);
        Data.AMODULE_fon[0]++;
        Data.AMODULE_find[0]++;
      }
    } else
    {
      spect_impulse = DISABLE;
      Data.AMODULE_timend = TIM_GetCapture2(TIM2);

      if(Data.AMODULE_timend > Data.AMODULE_timstart)
      {
        i = (Data.AMODULE_timend - Data.AMODULE_timstart) - 1;
      } else if(Data.AMODULE_timend < Data.AMODULE_timstart)
      {
        i = ((0xFFFF - Data.AMODULE_timstart) + Data.AMODULE_timend) - 1;
      } else
      {
        i = 0;
      }
//////////////////////////////////////////////////      

      i = i - Settings.AMODUL_spect_start;
      i = i / Settings.AMODUL_spect_multi;

      if(i < 100)
      {
        Data.AMODULE_len[i]++;  // Фон Модуля-А
      }
    }
    if(Isotop_counts >= Settings.Isotop_counts)
    {
      sound_activate();
      Isotop_counts = 0;
    } else
    {
      Isotop_counts++;
    }

  }
}

////////////////////////
void TIM4_IRQHandler(void)
{
  uint32_t i = 0;
  if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

    if(Settings.AMODUL_mode != 0)
    {

      Data.AMODULE_find_summ = Data.AMODULE_find[0];
      for (i = 9; i > 0; i--)
      {
        Data.AMODULE_find_summ += Data.AMODULE_find[i];
        Data.AMODULE_find[i] = Data.AMODULE_find[i - 1];
      }
      Data.AMODULE_find[0] = 0;

      if(Settings.AMODUL_unit == 2)
        DataUpdate.Need_display_update = ENABLE;

      if(Settings.AMODUL_Alarm_level > 0)
        if(Data.AMODULE_find_summ > Settings.AMODUL_Alarm_level_raw)
        {
          if(Alarm.Alarm_active == DISABLE)
            Alarm.User_cancel = DISABLE;

          Alarm.Alarm_active = ENABLE;
          sound_activate();

        } else
        {
          if(Alarm.Alarm_active == ENABLE)
          {
            sound_deactivate();
            Power.Sound_active = DISABLE;
            Alarm.Alarm_active = DISABLE;
            Alarm.User_cancel = DISABLE;
            Alarm.Alarm_beep_count = 0;
          }
        }
    }
  }
}




////////////////////////////////////////
// Секундный тик
////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RTC_Alarm_IRQHandler(void)
{                               // Тик каждые 4 секунды
  int i;

  EXTI_ClearITPendingBit(EXTI_Line17);

  // Модуль-А
  if(RTC_GetITStatus(RTC_IT_ALRB) != RESET)
  {
    RTC_ClearITPendingBit(RTC_IT_ALRB);

    if(!poweroff_state)
    {
      Set_next_B_alarm_wakeup();        // установить таймер просыпания на +1 секунду

      if(Settings.AMODUL_mode != 0)     // Если модуль-А активирован, обрабатываем его массив
      {
        for (i = 59; i > 0; i--)
        {
          Data.AMODULE_fon[i] = Data.AMODULE_fon[i - 1];
        }

        Data.AMODULE_fon[0] = 0;

        if((Data.AMODULE_fon[0] == 0) && (Data.AMODULE_fon[1] == 0) && (Data.AMODULE_fon[2] == 0))      // Если от модуля-А не поступает данных три цыкла, деактивируем его.
        {

          Settings.AMODUL_mode = 0;
          Data.modul_menu_select = 0;
          Data.menu_select = 0;
          Settings.AMODUL_menu = 0;
          Data.enter_menu_item = DISABLE;

          RTC_AlarmCmd(RTC_Alarm_B, DISABLE);
          RTC_ITConfig(RTC_IT_ALRB, DISABLE);
          RTC_ClearFlag(RTC_FLAG_ALRBF);

          TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);
          TIM_Cmd(TIM4, DISABLE);
          RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, DISABLE);


        }
        DataUpdate.Need_display_update = ENABLE;
      }

      Power.sleep_time = Settings.Sleep_time;
    }

  }
  // Основной счет времени
  if(RTC_GetITStatus(RTC_IT_ALRA) != RESET)
  {
    RTC_ClearITPendingBit(RTC_IT_ALRA);
    if(!poweroff_state)
    {
      Set_next_alarm_wakeup();  // установить таймер просыпания на +4 секунды

      if(Settings.AMODUL_mode == 0)
        DataUpdate.Need_display_update = ENABLE;

      // Калибровка 4 интервала счета
      if(Settings.Cal_mode == 1)
      {
        if(Cal_count < 20)
        {
          if(Settings.Second_count > Cal_count_time)
          {
            Cal_count_time += 4;
            Data.Cal_count_mass[Cal_count] += Detector_massive[Detector_massive_pointer];
          } else
          {
            Cal_count_time = 0;
            Cal_count++;
          }
        }
      }

      if(Power.USB_active)
      {
        Data.USB_not_active++;  // Счетчик неактивности USB
        if(Settings.AB_mode < 2)
          Data.madorc_impulse += Detector_massive[Detector_massive_pointer];    // Счетчик импульсов для передачи по USB
      }
      // Счетчик времени для обновления напряжения АКБ (каждые 4 минуты)
      if(DataUpdate.Batt_update_time_counter > 75)
      {
        DataUpdate.Need_batt_voltage_update = ENABLE;
        DataUpdate.Batt_update_time_counter = 0;
      } else
        DataUpdate.Batt_update_time_counter++;

      if((DataUpdate.Batt_update_time_counter > 2) && (Power.Display_active == ENABLE)) // Если дисплей активен, обновлять каждые 12 секунд
      {
        DataUpdate.Batt_update_time_counter = 0;
        DataUpdate.Need_batt_voltage_update = ENABLE;
      }
      // Счетчик времени для обновления счетчика импульсов накачки
      if(DataUpdate.pump_counter_update_time > 14)
      {
        if(!Power.USB_active)
          Data.madorc_impulse = 0;

        PumpData.pump_counter_avg_impulse_by_1sec[1] = PumpData.pump_counter_avg_impulse_by_1sec[0];
        PumpData.pump_counter_avg_impulse_by_1sec[0] = 0;
        DataUpdate.pump_counter_update_time = 0;

      } else
        DataUpdate.pump_counter_update_time++;

      // Счетчик дней
      if(DataUpdate.days_sec_count >= 24600)    // каждые 24 часа минут
      {
        DataUpdate.days_sec_count = 0;
        Data.working_days++;

      } else
        DataUpdate.days_sec_count++;
      // Сдвиг массива дозы
      if(DataUpdate.doze_sec_count >= 150)      // каждые 10 минут (150)
      {
        if(bat_cal_running > 0) // Запись калибровки АКБ
          cal_write();

        if(DataUpdate.Need_erase_flash == ENABLE)
        {
          full_erase_flash();
          DataUpdate.Need_erase_flash = DISABLE;
        }
        DataUpdate.Need_update_mainscreen_counters = ENABLE;

        // -----------------------------------------------------
        DataUpdate.doze_count++;
        if(DataUpdate.doze_count >= doze_length)        // Запись страницы во Flash
          //if(DataUpdate.doze_count>1) // Запись страницы во Flash
        {
          DataUpdate.doze_count = 0;
          flash_write_page(DataUpdate.current_flash_page);
          DataUpdate.current_flash_page++;
          if(DataUpdate.current_flash_page > (FLASH_MAX_PAGE))  // если за границами диапазона
            DataUpdate.current_flash_page = 0;
        }
        // -----------------------------------------------------

        for (i = doze_length; i > 0; i--)
        {
          ram_Doze_massive[i] = ram_Doze_massive[i - 1];        // сдвиг массива дозы
          ram_max_fon_massive[i] = ram_max_fon_massive[i - 1];  // сдвиг массива максимального фона
        }
        ram_Doze_massive[0] = 0;
        ram_max_fon_massive[0] = 0;
        DataUpdate.doze_sec_count = 1;  //// !!!!! 0

      } else
        DataUpdate.doze_sec_count++;
      ////////////////////////////////////////////////////    

      if(Settings.AB_mode == 2)
      {
        Data.AB_fon = calc_ab();

        for (i = 14; i > 0; i--)
        {
          Detector_AB_massive[i] = Detector_AB_massive[i - 1];
        }
        Detector_AB_massive[0] = 0;
      }
      ////////////////////////////////////////////////////    
      // Ускорение
      if(Settings.AB_mode < 2)
        if(Detector_massive[Detector_massive_pointer] >= 10 && Settings.Speedup == 1)
        {
          Data.auto_speedup_factor = 1;
          if(Detector_massive[Detector_massive_pointer] > 300)  // деление на 9 при фоне более 10 000
          {
            if(Data.auto_speedup_factor != 99)
              Data.auto_speedup_factor = 99;
          } else
          {
            if(Detector_massive[Detector_massive_pointer] > 199)        // деление на 9 при фоне более 10 000
            {
              if(Data.auto_speedup_factor != 30)
                Data.auto_speedup_factor = 30;
            } else
            {
              if(Detector_massive[Detector_massive_pointer] > 99)       // деление на 5 при фоне более 5 000
              {
                if(Data.auto_speedup_factor != 10)
                  Data.auto_speedup_factor = 10;
              } else
              {
                if(Detector_massive[Detector_massive_pointer] > 19)     // деление на 3 при фоне более 1 000
                {
                  if(Data.auto_speedup_factor != 4)
                    Data.auto_speedup_factor = 4;
                } else
                {               // деление на 2 при фоне более 500
                  if(Data.auto_speedup_factor != 2)
                    Data.auto_speedup_factor = 2;
                }
              }
            }
          }

          if(Data.auto_speedup_factor > (Settings.Second_count >> 3))
            Data.auto_speedup_factor = (Settings.Second_count >> 3);    // пересчет фона, если активированно ускорение
          if(Data.auto_speedup_factor != 1)
            recalculate_fon();  // пересчет фона, если активированно ускорение

        } else
        {                       // если ускорение не требуется
          if(Data.auto_speedup_factor != 1)
          {
            Data.auto_speedup_factor = 1;
            recalculate_fon();
          } else
          {
            if(Settings.AB_mode < 2)
              Data.fon_level += Detector_massive[Detector_massive_pointer];
          }
        }

      if(Settings.AB_mode < 2)
      {
        Detector_massive_pointer++;
        if(Detector_massive_pointer >= (Settings.Second_count >> 2))
        {
          if(Data.auto_speedup_factor == 1)
            Data.fon_level -= Detector_massive[0];
          Detector_massive[0] = 0;
          Detector_massive_pointer = 0;
        } else
        {
          if(Data.auto_speedup_factor == 1)
            Data.fon_level -= Detector_massive[Detector_massive_pointer];
          Detector_massive[Detector_massive_pointer] = 0;
        }
        if(Data.fon_level > ram_max_fon_massive[0])
          ram_max_fon_massive[0] = Data.fon_level;      // заполнение массива максимального фона
      }
      DataUpdate.Need_fon_update = ENABLE;
      ////////////////////////////////////////////////////

      if(Power.sleep_time > 4)
      {
        Power.sleep_time -= 4;
      } else
      {
        Power.sleep_time = 0;
      }
    }
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/


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
