#include "pump.h"
#include "main.h"

#pragma O0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void COMP_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line22) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line22);

//    if(COMP_GetOutputLevel(COMP_Selection_COMP2) == COMP_OutputLevel_High)
    {
      PumpCmd(DISABLE);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PumpCmd(FunctionalState pump)
{

  if(pump == ENABLE)
  {
    if((PumpData.Active == DISABLE) && (!PUMP_DEAD_TIME))
    {
      PumpData.Active = pump;

      PUMP_DEAD_TIME = ENABLE;  // начинаем отсчет мертвого времени накачки

      TIM3->EGR |= 0x0001;      // ”станавливаем бит UG дл€ принудительного сброса счетчика

      TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
      TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);

      TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Enable);  // разрешить накачку   
      TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
      TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }

  } else
  {
    TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Disable);   // запретить накачку
    PumpData.Active = pump;
  }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




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

      while (PWR_GetFlagStatus(PWR_FLAG_VREFINTRDY) != ENABLE); // ∆дем нормализации референса
      COMP_Init(&COMP_InitStructure);
      while (!(COMP->CSR & COMP_CSR_INSEL));    // ждем пока не включитьс€ компаратор

      EXTI_StructInit(&EXTI_InitStructure);
      EXTI_InitStructure.EXTI_Line = EXTI_Line22;
      EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
      EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
      EXTI_InitStructure.EXTI_LineCmd = ENABLE;
      EXTI_Init(&EXTI_InitStructure);

      NVIC_InitStructure.NVIC_IRQChannel = COMP_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
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


      while (PWR_GetFlagStatus(PWR_FLAG_VREFINTRDY) != ENABLE); // ∆дем нормализации референса
      COMP_Init(&COMP_InitStructure);
      while (!(COMP->CSR & COMP_CSR_INSEL));    // ждем пока не включитьс€ компаратор
      EXTI_ClearITPendingBit(EXTI_Line22);

      break;
    }

  case OFF_COMP:
    {

      COMP_DeInit();
      EXTI_ClearITPendingBit(EXTI_Line22);
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_COMP, DISABLE);

      break;
    }
  default:
    {
      break;
    }
  }
}
