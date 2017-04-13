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
      PumpCmd(ENABLE);
    } else
    {
      PumpCmd(DISABLE);
    }

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

    if((PumpData.Active == DISABLE) && (PUMP_DEAD_TIME == DISABLE))     // ���� ���������� ������� �� ��������� �������
    {
      address = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);

      if(address < 12)
        return;

      if(!PowerState.Spectr)
        GPIO_SetBits(GPIOA, GPIO_Pin_4);        //  ����� ������� �� ������� ����������

      SPECTRO_MASSIVE[address >> 1]++;  // ���� ��� ���������, ��������� ������

      IMPULSE_MASSIVE[0]++;     // ����������� �������

      if((tmpadc1 > Settings.Sound) && (Settings.Sound > 0) && PowerState.Sound)
      {
        tmpadc1 = 0;
        TIM4->EGR |= 0x0001;    // ������ ����
        TIM_CCxCmd(TIM4, TIM_Channel_4, TIM_CCx_Enable);
        TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

      } else
        tmpadc1++;
      if(!PowerState.Spectr)
        GPIO_ResetBits(GPIOA, GPIO_Pin_4);      //  ����� ������� �� ������� ����������
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��� 1 �������
void RTC_WKUP_IRQHandler(void)
{
  EXTI_ClearITPendingBit(EXTI_Line20);
  {
    if(RTC_GetITStatus(RTC_IT_WUT) != RESET)
    {
      RTC_ClearITPendingBit(RTC_IT_WUT);
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
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �������
int tmptim3;
void TIM3_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    if(tmptim3 > 0)
    {
      if(PumpData.Active == DISABLE)    // ���� ������� ���� ���������
      {
        tmptim3 = 0;
        TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
        TIM_ITConfig(TIM3, TIM_IT_CC2, DISABLE);
        TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Disable);       // ��������� �������
        PUMP_DEAD_TIME = DISABLE;
      }
    } else
      tmptim3++;

  }

  if(TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);

//    PUMP_DEAD_TIME = ENABLE;    // �������� ������ �������� ������� �������
//    TIM10->EGR |= 0x0001;
//    TIM_ITConfig(TIM10, TIM_IT_CC1, ENABLE);
  }

  PUMP_MASSIVE[0]++;
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
void TIM9_IRQHandler(void)
{
  int i;
  if(TIM_GetITStatus(TIM9, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM9, TIM_IT_Update);

    if(PowerState.Spectr)
      spectro_time++;           // ���� ������� ���������� �������


    // -----------------------------
    // ������� ��������
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
      sprintf(LED_BUF, "%3u", counter / 1000);  // ����� � ����� �������� ��������
    } else
    {
      sprintf(LED_BUF, "%3u", counter); // ����� � ����� �������� ��������
    }
    LEDString();                // // ������� ������� ������� ���������� ������

    // ��������� ����������         
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
  int i;
  if(EXTI_GetITStatus(EXTI_Line8) != RESET)
  {
    for (i = 0; i < 10000; i++) // ���������� �������� ������
    {
      while (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8));
    }

    if(PowerState.USB)
    {
      // ���� �������� USB �����, � ����������� �� �������, �������� �����������
      if(!PowerState.Spectr)
      {
        PowerState.Spectr = ENABLE;
        dac_on();               // ��������� ���
        PumpCompCmd(INIT_COMP); // ��������� �����������
        PumpCompCmd(ON_COMP);
        TIM_Cmd(TIM10, ENABLE); // ��������� �������� ���������� �� ���

        TIM_Cmd(TIM2, DISABLE); // ��������� ���������
        LED_show(LED_show_massive[0], C_SEG_ALLOFF);

        PowerState.Sound = DISABLE;     // ���� ���������
      } else
        // ���� �������� USB �����, � ����������� �������, ��������� �����������
      {
        PowerState.Spectr = DISABLE;
        dac_off();              // ���������� ���
        PumpCompCmd(OFF_COMP);  // ���������� �����������
        TIM_Cmd(TIM10, DISABLE);        // ���������� �������� ���������� �� ���
        PumpCmd(DISABLE);

        TIM_Cmd(TIM2, DISABLE); // ��������� ���������
        LED_show(LED_show_massive[0], C_SEG_ALLOFF);

        PowerState.Sound = DISABLE;     // ���� ���������
      }
    } else
    {
      if(PowerState.Spectr == DISABLE)
      {
        PowerState.Spectr = ENABLE;
        Power_off();
      } else
      {
        PowerState.Spectr = DISABLE;
        Power_on();
      }
    }

/*		
    {
      // ���� ��������� USB �����, � ����������� �� �������, �������� �����������
      if(!PowerState.Spectr)
      {
        PowerState.Spectr = ENABLE;
        dac_on();               // ��������� ���
        PumpCompCmd(INIT_COMP); // ��������� �����������
        PumpCompCmd(ON_COMP);
        TIM_Cmd(TIM10, ENABLE); // ��������� �������� ���������� �� ���

        TIM_Cmd(TIM2, DISABLE); // ��������� ���������
        LED_show(LED_show_massive[0], C_SEG_ALLOFF);
        LED_show(1, C_SEG_A);   // �������� ��� ��������� 1 �������

        PowerState.Sound = DISABLE;     // ���� ���������
      } else
        // ���� ��������� USB �����, � ����������� �������, ������� � ����� ������
      {
        PowerState.Spectr = DISABLE;
        dac_on();               // ��������� ���
        PumpCompCmd(INIT_COMP); // ��������� �����������
        PumpCompCmd(ON_COMP);
        TIM_Cmd(TIM10, ENABLE); // ��������� �������� ���������� �� ���

        TIM_Cmd(TIM2, ENABLE);  // ��������� ��������
        LED_show(LED_show_massive[0], C_SEG_ALLOFF);

        PowerState.Sound = ENABLE;      // ���� ��������
      }
    }
*/

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
