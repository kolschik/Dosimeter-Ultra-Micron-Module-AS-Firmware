#include "pump.h"
#include "main.h"

//#pragma O0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COMP_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line22) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line22);

    if(COMP_GetOutputLevel(COMP_Selection_COMP2) == COMP_OutputLevel_High)
      //{
      PumpCmd(DISABLE);
    //} else
    //{
    // PumpCmd(ENABLE);
    //}

  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PumpCmd(FunctionalState pump)
{

  if(pump == ENABLE)
  {
    //if((PumpData.Active == DISABLE) && (!PUMP_DEAD_TIME) && (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)))
    if((PumpData.Active == DISABLE) && (!PUMP_DEAD_TIME))
    {
      PumpData.Active = pump;

      PUMP_DEAD_TIME = ENABLE;  // начинаем отсчет мертвого времени накачки

      TIM3->EGR |= 0x0001;      // Устанавливаем бит UG для принудительного сброса счетчика

      TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
      TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);

      TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Enable);  // разрешить накачку   
      TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
      TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }

  } else
  {
    TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Disable);   // запретить накачку
    //TIM_ITConfig(TIM3, TIM_IT_CC2, DISABLE);
    //TIM3->EGR |= 0x0001;        // Устанавливаем бит UG для принудительного сброса счетчика
    //TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
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
     {                             // Если подано внешнее питание
     pump_period = (Settings.v4_target_pump * 4200) / 4400;      // расчет целевой накачки (Пример 1,75мкс*4.2В/4.4В напряжение USB=1.25мкс)
     } else
     {                             // Если питается от батарейки
     pump_period = (Settings.v4_target_pump * 4200) / voltage;   // расчет целевой накачки (Пример 1,75мкс*4.2В/3.3В напряжение АКБ=2.0мкс)
     }

     TIM_PrescalerConfig(TIM9, (uint16_t) ((SystemCoreClock / 4000000) - 1), TIM_PSCReloadMode_Update);    // 0.25 мкс
     TIM_SetCompare1(TIM9, pump_period);   // перерасчет энергии накачки

     RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);  // 1 tick is 488 us
     RTC_SetWakeUpCounter(0x20);   // Установить таймаут просыпания
     while (RTC_WakeUpCmd(ENABLE) != SUCCESS);
     RTC_ITConfig(RTC_IT_WUT, ENABLE);

   */
}

/////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
void PumpTimerConfig(void)      // генерация накачки
{
/*  TIM_TimeBaseInitTypeDef TIM_BaseConfig;
  TIM_OCInitTypeDef TIM_OCConfig;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);

  TIM_OCStructInit(&TIM_OCConfig);
  TIM_TimeBaseStructInit(&TIM_BaseConfig);

  TIM_OCConfig.TIM_OCMode = TIM_OCMode_PWM1;    // Конфигурируем выход таймера, режим - PWM1
  TIM_OCConfig.TIM_OutputState = TIM_OutputState_Enable;        // Собственно - выход включен
  TIM_OCConfig.TIM_Pulse = Settings.v4_target_pump;
  TIM_OCConfig.TIM_OCPolarity = TIM_OCPolarity_High;    // Полярность => пульс - это единица (+3.3V)

  TIM_BaseConfig.TIM_Prescaler = (uint16_t) (SystemCoreClock / 4000000) - 1;    // Делитель (1 тик = 0.25мкс)
  TIM_BaseConfig.TIM_ClockDivision = 0;
  TIM_BaseConfig.TIM_Period = 560;      // ИЗМЕРЕННО ОСЦЫЛОМ, 560!  Общее количество тиков (скваженность) 140мкс (частота накачки 1с/140мкс=** кГц)
  TIM_BaseConfig.TIM_CounterMode = TIM_CounterMode_Up;  // Отсчет от нуля до TIM_Period


  TIM_DeInit(TIM9);             // Де-инициализируем таймер №9

  TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM9, ENABLE);

  TIM_TimeBaseInit(TIM9, &TIM_BaseConfig);
  TIM_OC1Init(TIM9, &TIM_OCConfig);     // Инициализируем первый выход таймера №9 (у HD это PB13)

  TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM9, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel = TIM9_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_ITConfig(TIM9, TIM_IT_CC1, ENABLE);

  TIM9->EGR |= 0x0001;          // Устанавливаем бит UG для принудительного сброса счетчика
  TIM_CCxCmd(TIM9, TIM_Channel_1, TIM_CCx_Disable);     // запретить накачку

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

      while (PWR_GetFlagStatus(PWR_FLAG_VREFINTRDY) != ENABLE); // Ждем нормализации референса
      COMP_Init(&COMP_InitStructure);
      while (!(COMP->CSR & COMP_CSR_INSEL));    // ждем пока не включиться компаратор

      EXTI_StructInit(&EXTI_InitStructure);
      EXTI_InitStructure.EXTI_Line = EXTI_Line22;
      EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
      EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
      EXTI_InitStructure.EXTI_LineCmd = ENABLE;
      EXTI_Init(&EXTI_InitStructure);

      NVIC_InitStructure.NVIC_IRQChannel = COMP_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
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


      while (PWR_GetFlagStatus(PWR_FLAG_VREFINTRDY) != ENABLE); // Ждем нормализации референса
      COMP_Init(&COMP_InitStructure);
      while (!(COMP->CSR & COMP_CSR_INSEL));    // ждем пока не включиться компаратор
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
