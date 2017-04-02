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
uint16_t tmpadc1;
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


      SPECTRO_MASSIVE[address >> 1]++;
      IMPULSE_MASSIVE[0]++;     // увеличиваем счетчик


      if(tmpadc1 > 5)
      {
        tmpadc1 = 0;
        TIM4->EGR |= 0x0001;    // Подаем звук
        TIM_CCxCmd(TIM4, TIM_Channel_4, TIM_CCx_Enable);
        TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

      } else
        tmpadc1++;
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
int tmptim9;
void TIM9_IRQHandler(void)
{
  int i;
  if(TIM_GetITStatus(TIM9, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM9, TIM_IT_Update);

    PumpData.Impulse_past = DISABLE;

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

    // Антиалиасинг ячеек кратных 64
    for (i = 1; i <= 16; i++)
    {
      SPECTRO_MASSIVE[(i << 6) - 1] = (SPECTRO_MASSIVE[(i << 6) - 2] + SPECTRO_MASSIVE[(i << 6)]) / 2;
    }

    if(tmptim9 > 50)
    {
      tmptim9 = 0;
      adc_calibration();
      dac_reload();
    } else
      tmptim9++;


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
