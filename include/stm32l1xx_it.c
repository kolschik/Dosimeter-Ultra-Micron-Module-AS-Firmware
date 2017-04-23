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
  msTicks++;                    // ��������� �������� �������
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
void TIM2_IRQHandler(void)      // ���������� �������
{
  if(TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);

    LED_show(LED_show_massive[0], C_SEG_ALLOFF);        // ��������� ��� ��������
  }


  if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    Need_Ledupdate = ENABLE;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TIM10_IRQHandler(void)     // ������������ �������� ����������
{
  if(TIM_GetITStatus(TIM10, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM10, TIM_IT_Update);


    if(COMP_GetOutputLevel(COMP_Selection_COMP2) == COMP_OutputLevel_Low)
    {
      if(PumpData.Agressive)    // ���� ������� ���� ������ ���������������
        PumpCmd(ENABLE);

      pump_per_second_mass[0]++;        // �������������� ���� ��������� �����������
    } else
    {
      if(PumpData.Agressive)    // ���� ������� ���� ������ ���������������
        PumpCmd(DISABLE);

      pump_per_second_mass[0]--;        // �������������� ���� ��������� �����������
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t tmpadc1;
void ADC1_IRQHandler(void)
{
  uint16_t address;

  // � ������� ������ ��������� ������ ������ 5.2 ���
  if(ADC_GetITStatus(ADC1, ADC_IT_JEOC) != RESET)
  {
    ADC_ClearITPendingBit(ADC1, ADC_IT_JEOC);

    // � ������� ������ ��������� ������ ������ 7 ���
    address = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);

    if((!PumpData.Active) && (!PUMP_DEAD_TIME)) // ���� ���������� ������� �� ��������� �������
    {
      if(address < (Settings.Start_channel << 1))       // ������� ��� � ���������� �������
      {
        return;
      }


      if(PowerState.Spectr)
      {
        // ����� ������������
        SPECTRO_MASSIVE[address >> 1]++;        // ���� ��� ���������, ��������� ������
        IMPULSE_MASSIVE[0]++;   // ����������� �������                                
      } else
      {
        GPIOA->BSRRL = GPIO_Pin_4;      //  ����� ������� �� ������� ���������� - ������ ������
        IMPULSE_MASSIVE[0]++;   // ����������� �������
        if((tmpadc1 > Settings.Sound) && (Settings.Sound > 0) && PowerState.Sound)
        {
          tmpadc1 = 0;
          TIM4->EGR |= 0x0001;  // ������ ����
          TIM_CCxCmd(TIM4, TIM_Channel_4, TIM_CCx_Enable);
          TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

        } else
        {
          tmpadc1++;
        }
        GPIOA->BSRRH = GPIO_Pin_4;      //  ����� ������� �� ������� ���������� - ����� ������
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �������
uint32_t tmptim3 = 0, counts;

void TIM3_IRQHandler(void)
{
  uint32_t i;
  if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

    // ��������, ������� �� ����� �������
    i = TIM3->CCER & (uint16_t) (TIM_CCx_Enable << TIM_Channel_2);
    if(i > 0)
      PUMP_DEAD_TIME = ENABLE;  // �������� ������ �������� ������� �������


    if(PumpData.Agressive)      // ���� ������� ���� ������ ���������������
    {

      if(PumpData.Active == DISABLE)    // ���� ������� ���� ���������
      {
        if(tmptim3 > 0)
        {
          tmptim3 = 0;
          //TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
          //TIM_ITConfig(TIM3, TIM_IT_CC2, DISABLE);
          TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Disable);     // ��������� �������
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
      PUMP_DEAD_TIME = DISABLE; // ����������� ������ �������� ������� �������
    }
  }
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����
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
// 0.1 �������
int tim9cnt, tim9cnt_spectr_heatup;
void TIM9_IRQHandler(void)
{
  uint32_t i;
  if(TIM_GetITStatus(TIM9, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM9, TIM_IT_Update);

    if(PowerState.Spectr)
      spectro_time++;           // ���� ������� ���������� �������

    if((!PowerState.USB) && (PowerState.Spectr))        // ���������� ���� �������
    {
      if(tim9cnt_spectr_heatup < 600)   // ���� 60 ������
      {
        tim9cnt_spectr_heatup++;
        if((tim9cnt_spectr_heatup % 10) == 0)   // ������ ���� ������ ������� � ������ �����������
        {
          for (i = 0; i < 1000; i++)
          {
            TIM_Cmd(TIM2, DISABLE);     // ��������� ���������
            TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
            TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

            LED_show(LED_show_massive[0], C_SEG_ALLOFF);
            LED_show(1, C_SEG_A);       // �������� ��� ��������� 1 �������

            TIM_CCxCmd(TIM4, TIM_Channel_4, TIM_CCx_Enable);
          }
          LED_show(LED_show_massive[0], C_SEG_ALLOFF);
          TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);    //��������� ����
        }
      } else
      {
        if(!PowerState.Engage)
        {
          LED_show(1, C_SEG_A); // �������� ��� ��������� 1 �������
          spectro_time = 0;     // ����� ����� ������� ���������� �������
          for (i = 0; i <= 2047; i++)   // ����� ������� �������
            SPECTRO_MASSIVE[i] = 0;
          PowerState.Engage = ENABLE;   // ��������� ���� �������
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

    if(PowerState.key_hold > 20)        // ���� ������ ������ 3 ������� - ���������� ����������
    {
      PowerState.Off_mode = ENABLE;
    }
    // -----------------------------
    // ������� ��������
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
      PumpData.two_sec_sum += pump_per_second_mass[i - 1];      // ����� �� ��� �������

    }
    counter >>= 1;              // ������� �� ���
    counter_pump >>= 1;         // ������� �� ���
    IMPULSE_MASSIVE[0] = 0;
    PUMP_MASSIVE[0] = 0;

    pump_per_second_mass[0] = 0;

    // ���� ��������� ������������ �����������, ����������� � ����� ������� �������
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
      if(PumpData.good_stable_pumps > 100)      // 10 ������ ������� ���������
      {
        PumpCompCmd(NVIC_DEINIT);
        PumpData.Average_pump = counter_pump;
        counts = counter_pump;
        PumpData.Agressive = DISABLE;
        TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Enable);        // ��������� �������   
      }
    }
    // -----------------------------

    if(!PumpData.Agressive)     // ���� ������� ���� ����� ���������������
    {
      pump_per_second = counts + ((pump_per_second_mass[1] + pump_per_second_mass[2] + pump_per_second_mass[3]) / 50);


      // ��������� ������� � ���� ������������ (+-100 ���/c, �� �� ����� 50-�� ���������)
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

      // ��������� ������ �������� ������� �������
      i = timer_freq / (pump_per_second);
      if(i > 0xFFFF)
        i = 0xFFFF;

      TIM_SetAutoreload(TIM3, i);
      counts = counter_pump;
      // -----------------------------
    }
    // -----------------------------


    if(tim9cnt >= 25)           // ������ 2.5 �������
    {
      tim9cnt = 0;
      need_MCP_update = ENABLE; // ���� ��������� USB

      if(PowerState.USB)
        LED_show(LED_show_massive[0], C_SEG_ALLOFF);

      if(akb_voltage < 340)     // ������ 3.4 ������, ��������� ����������!
      {
        if(!PowerState.Low_bat)
        {
          PowerState.Low_bat = ENABLE;
        } else
        {
          // ������������ ���������� ��� ����� � ���
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
        sprintf(LED_BUF, "%3u", counter / 1000);        // ����� � ����� �������� ��������
      } else
      {
        sprintf(LED_BUF, "%3u", counter);       // ����� � ����� �������� ��������
      }
    } else
    {
      sprintf(LED_BUF, "Lo ");
    }

    LEDString();                // // ������� ������� ������� ���������� ������
    // ��������� ����������         
    if(PumpData.Agressive)      // ���� ������� ���� ������ ���������������
      if(COMP_GetOutputLevel(COMP_Selection_COMP2) == COMP_OutputLevel_High)
      {
        PumpCmd(DISABLE);
      }


  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������� ���������� ������� ���
void DMA1_Channel1_IRQHandler(void)
{
  int i;
  uint16_t ts_cal1, ts_cal2, TS_data_norm, Vrefint_cal;
  float Vdda, temperature_norm, tmp;

  if(DMA_GetITStatus(DMA1_IT_TC1))
  {
    DMA_ClearITPendingBit(DMA1_IT_GL1);

    // ���������� 128-� ������� 
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

    tmp = 1.224 / (float) ADCData.Ref_voltage_raw;      // ������� �������� �����. ���������� ��������� 1.22�, �� ���� ��������� �������� ������������ ������������� 1 ���.
    // 0,0007326� �� 1 ���

    if(ADCData.Calibration_bit_voltage != tmp)  // ���� ��� ���������� ���������, ����������� ���
    {
      ADCData.Calibration_bit_voltage = tmp;
      dac_reload();
    }
    // ���������� ���������� ���
    akb_voltage = (float) (ADCData.Calibration_bit_voltage * ADCData.Batt_voltage_raw * 100 * 2);


    ts_cal1 = *((uint16_t *) ((uint32_t) 0x1FF8007A));
    ts_cal2 = *((uint16_t *) ((uint32_t) 0x1FF8007E));
    Vrefint_cal = *((uint16_t *) ((uint32_t) 0x1FF80078));

    Vdda = 3 * ((float) Vrefint_cal / ADCData.Ref_voltage_raw);
    TS_data_norm = ADCData.Temp_voltage_raw * Vdda / 3;
    temperature_norm = (((float) (110 - 30) / (ts_cal2 - ts_cal1)) * (TS_data_norm - ts_cal1) + 30);    //� �������������

    ADCData.Temp = temperature_norm;

  }
}

///////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////
// =======================================================
// ���������� �� ������
void EXTI9_5_IRQHandler(void)
{
  uint32_t i = 0, jitter = 0;
  if(EXTI_GetITStatus(EXTI_Line8) != RESET)
  {
    if(!sleep_rtc)
    {
      LED_show(LED_show_massive[0], C_SEG_ALLOFF);
      for (i = 0; i < 100000; i++)      // ���������� �������� ������
      {
        while (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8))
        {
          if(jitter > 60000)
            TIM_CCxCmd(TIM4, TIM_Channel_4, TIM_CCx_Enable);    // ���� ������ ������, ������ ����

          jitter++;
        }
      }
      TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);        //���� ������, ��������� ����

      if(jitter > 60000)
      {
        if(PowerState.USB)
        {
          // ���� �������� USB �����, ��������� ���������, �������� �����������
          PowerState.Spectr = ENABLE;
          dac_on();             // ��������� ���
          PumpCompCmd(INIT_COMP);       // ��������� �����������
          PumpCompCmd(ON_COMP);
          TIM_Cmd(TIM10, ENABLE);       // ��������� �������� ���������� �� ���

          TIM_Cmd(TIM2, DISABLE);       // ��������� ���������
          TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
          TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
          LED_show(LED_show_massive[0], C_SEG_ALLOFF);

          PowerState.Sound = DISABLE;   // ���� ���������
        } else
        {
          // ���� ��������� USB �����, � ����������� �� �������, �������� �����������
          if(!PowerState.Spectr)
          {
            PowerState.Spectr = ENABLE;
            tim9cnt_spectr_heatup = 0;
            PowerState.Engage = DISABLE;
            dac_on();           // ��������� ���
            PumpCompCmd(INIT_COMP);     // ��������� �����������
            PumpCompCmd(ON_COMP);
            TIM_Cmd(TIM10, ENABLE);     // ��������� �������� ���������� �� ���

            TIM_Cmd(TIM2, DISABLE);     // ��������� ���������
            TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
            TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
            LED_show(LED_show_massive[0], C_SEG_ALLOFF);
            LED_show(1, C_SEG_A);       // �������� ��� ��������� 1 �������

            PowerState.Sound = DISABLE; // ���� ���������
          } else
            // ���� ��������� USB �����, � ����������� �������, ������� � ����� ������
          {
            PowerState.Spectr = DISABLE;
            dac_on();           // ��������� ���
            PumpCompCmd(INIT_COMP);     // ��������� �����������
            PumpCompCmd(ON_COMP);
            TIM_Cmd(TIM10, ENABLE);     // ��������� �������� ���������� �� ���

            TIM_Cmd(TIM2, ENABLE);      // ��������� ��������
            TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
            TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
            LED_show(LED_show_massive[0], C_SEG_ALLOFF);

            PowerState.Sound = ENABLE;  // ���� ��������

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
