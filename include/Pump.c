#include "pump.h"
#include "main.h"

#pragma O0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COMP_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line22) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line22);

    if(COMP_GetOutputLevel(COMP_Selection_COMP2) == COMP_OutputLevel_High)
    {
      PumpCmd(DISABLE);
    } else
    {
      //PumpCmd(ENABLE);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PumpCmd(FunctionalState pump)
{

  if(pump == ENABLE)
  {
    if((PumpData.Active == DISABLE) && (PumpData.Impulse_past == DISABLE))
    {
      PumpData.Active = pump;
      PumpData.Impulse_past = ENABLE;
      TIM3->EGR |= 0x0001;      // ������������� ��� UG ��� ��������������� ������ ��������
      TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Enable);  // ��������� �������   
      TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
      TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
      TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
      TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }

  } else
  {
    TIM_ITConfig(TIM3, TIM_IT_CC2, DISABLE);
    TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Disable);   // ��������� �������
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
    PumpData.Active = pump;
  }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PumpPrescaler(void)
{
  /*
     uint32_t pump_period = 0, voltage = 0;

     SystemCoreClockUpdate();

     if(Settings.v4_target_pump > 15)
     Settings.v4_target_pump = 8;
     if((ADCData.Batt_voltage < 2800) || (ADCData.Batt_voltage > 4200))
     {
     voltage = 4200;
     } else
     {
     voltage = ADCData.Batt_voltage;
     }

     if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9))
     {                             // ���� ������ ������� �������
     pump_period = (Settings.v4_target_pump * 4200) / 4400;      // ������ ������� ������� (������ 1,75���*4.2�/4.4� ���������� USB=1.25���)
     } else
     {                             // ���� �������� �� ���������
     pump_period = (Settings.v4_target_pump * 4200) / voltage;   // ������ ������� ������� (������ 1,75���*4.2�/3.3� ���������� ���=2.0���)
     }

     TIM_PrescalerConfig(TIM9, (uint16_t) ((SystemCoreClock / 4000000) - 1), TIM_PSCReloadMode_Update);    // 0.25 ���
     TIM_SetCompare1(TIM9, pump_period);   // ���������� ������� �������

     RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);  // 1 tick is 488 us
     RTC_SetWakeUpCounter(0x20);   // ���������� ������� ����������
     while (RTC_WakeUpCmd(ENABLE) != SUCCESS);
     RTC_ITConfig(RTC_IT_WUT, ENABLE);

   */
}

/////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
void PumpTimerConfig(void)      // ��������� �������
{
/*  TIM_TimeBaseInitTypeDef TIM_BaseConfig;
  TIM_OCInitTypeDef TIM_OCConfig;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);

  TIM_OCStructInit(&TIM_OCConfig);
  TIM_TimeBaseStructInit(&TIM_BaseConfig);

  TIM_OCConfig.TIM_OCMode = TIM_OCMode_PWM1;    // ������������� ����� �������, ����� - PWM1
  TIM_OCConfig.TIM_OutputState = TIM_OutputState_Enable;        // ���������� - ����� �������
  TIM_OCConfig.TIM_Pulse = Settings.v4_target_pump;
  TIM_OCConfig.TIM_OCPolarity = TIM_OCPolarity_High;    // ���������� => ����� - ��� ������� (+3.3V)

  TIM_BaseConfig.TIM_Prescaler = (uint16_t) (SystemCoreClock / 4000000) - 1;    // �������� (1 ��� = 0.25���)
  TIM_BaseConfig.TIM_ClockDivision = 0;
  TIM_BaseConfig.TIM_Period = 560;      // ��������� �������, 560!  ����� ���������� ����� (������������) 140��� (������� ������� 1�/140���=** ���)
  TIM_BaseConfig.TIM_CounterMode = TIM_CounterMode_Up;  // ������ �� ���� �� TIM_Period


  TIM_DeInit(TIM9);             // ��-�������������� ������ �9

  TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM9, ENABLE);

  TIM_TimeBaseInit(TIM9, &TIM_BaseConfig);
  TIM_OC1Init(TIM9, &TIM_OCConfig);     // �������������� ������ ����� ������� �9 (� HD ��� PB13)

  TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM9, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel = TIM9_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_ITConfig(TIM9, TIM_IT_CC1, ENABLE);

  TIM9->EGR |= 0x0001;          // ������������� ��� UG ��� ��������������� ������ ��������
  TIM_CCxCmd(TIM9, TIM_Channel_1, TIM_CCx_Disable);     // ��������� �������

  PumpCompCmd(INIT_COMP);

  TIM_Cmd(TIM9, ENABLE);
	*/
}

/////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////
void PumpCompCmd(uint8_t cmd)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  COMP_InitTypeDef COMP_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  switch (cmd)
  {
  case INIT_COMP:
    {
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_COMP, ENABLE);
      SYSCFG_RIIOSwitchConfig(RI_IOSwitch_GR6_1, ENABLE);

      COMP_InitStructure.COMP_InvertingInput = COMP_InvertingInput_DAC2;
      COMP_InitStructure.COMP_OutputSelect = COMP_OutputSelect_None;
      COMP_InitStructure.COMP_Speed = COMP_Speed_Fast;

      while (PWR_GetFlagStatus(PWR_FLAG_VREFINTRDY) != ENABLE); // ���� ������������ ���������
      COMP_Init(&COMP_InitStructure);
      while (!(COMP->CSR & COMP_CSR_INSEL));    // ���� ���� �� ���������� ����������

      EXTI_StructInit(&EXTI_InitStructure);
      EXTI_InitStructure.EXTI_Line = EXTI_Line22;
      EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
      EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
      EXTI_InitStructure.EXTI_LineCmd = ENABLE;
      EXTI_Init(&EXTI_InitStructure);

      NVIC_InitStructure.NVIC_IRQChannel = COMP_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&NVIC_InitStructure);
      EXTI_ClearITPendingBit(EXTI_Line22);

      break;
    }

  case ON_COMP:
    {
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_COMP, ENABLE);

      COMP_InitStructure.COMP_InvertingInput = COMP_InvertingInput_DAC2;
      COMP_InitStructure.COMP_OutputSelect = COMP_OutputSelect_None;
      COMP_InitStructure.COMP_Speed = COMP_Speed_Fast;


      while (PWR_GetFlagStatus(PWR_FLAG_VREFINTRDY) != ENABLE); // ���� ������������ ���������
      COMP_Init(&COMP_InitStructure);
      while (!(COMP->CSR & COMP_CSR_INSEL));    // ���� ���� �� ���������� ����������
      EXTI_ClearITPendingBit(EXTI_Line22);

      break;
    }

  case OFF_COMP:
    {

      COMP_DeInit();
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_COMP, DISABLE);
      EXTI_ClearITPendingBit(EXTI_Line22);

      break;
    }
  default:
    {
      break;
    }
  }
}