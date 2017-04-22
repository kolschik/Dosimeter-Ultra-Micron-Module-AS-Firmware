#include "stm32l1xx_tim.h"
#include "main.h"

///////////////////////////////////////////////////////////////////////////////////////////
void tim2_Config()              // LED индикация
{
  TIM_TimeBaseInitTypeDef TIM_BaseConfig;
  TIM_OCInitTypeDef TIM_OCConfig;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  TIM_TimeBaseStructInit(&TIM_BaseConfig);

  TIM_BaseConfig.TIM_Prescaler = (uint16_t) (SystemCoreClock / 20000) - 1;      // частота 10 кгц.
  TIM_BaseConfig.TIM_ClockDivision = 0;
  TIM_BaseConfig.TIM_Period = 40;       // Апдейт 250 Гц (полное обновление 3-х цифр 83 Гц)
  TIM_BaseConfig.TIM_CounterMode = TIM_CounterMode_Up;



  TIM_OCConfig.TIM_OCMode = TIM_OCMode_Timing;  // Конфигурируем выход таймера, режим - PWM1
  TIM_OCConfig.TIM_OutputState = TIM_OutputState_Enable;        // Собственно - выход включен
  TIM_OCConfig.TIM_Pulse = (Settings.LED_intens + 1) * 5;       // длительность импульса

  if(TIM_OCConfig.TIM_Pulse > TIM_BaseConfig.TIM_Period)
    TIM_OCConfig.TIM_Pulse = TIM_BaseConfig.TIM_Period;

  TIM_OCConfig.TIM_OCPolarity = TIM_OCPolarity_High;    // Полярность => пульс - это единица (+3.3V)

  TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM2, ENABLE);

  TIM_TimeBaseInit(TIM2, &TIM_BaseConfig);
  TIM_OC2Init(TIM2, &TIM_OCConfig);     // Инициализируем второй выход

  TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);


  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);

  TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);

  TIM2->EGR |= 0x0001;          // Устанавливаем бит UG для принудительного сброса счетчика
  TIM_Cmd(TIM2, ENABLE);

}

/////////////////////////////////////////////////////////////////////////////////



void tim3_Config()              //  
{
  TIM_TimeBaseInitTypeDef TIM_BaseConfig;
  TIM_OCInitTypeDef TIM_OCConfig;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  // Базовая настройка, тик 0.25 мкс, счет 14 мкс
  TIM_TimeBaseStructInit(&TIM_BaseConfig);
  TIM_BaseConfig.TIM_Prescaler = (uint16_t) (SystemCoreClock / 4000000) - 1;
  TIM_BaseConfig.TIM_ClockDivision = 0;
  TIM_BaseConfig.TIM_Period = 100;
  TIM_BaseConfig.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_BaseConfig);
  TIM_ARRPreloadConfig(TIM3, ENABLE);

  // Настройка вывода на накачку, 0.5 мкс
  TIM_OCStructInit(&TIM_OCConfig);
  TIM_OCConfig.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCConfig.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCConfig.TIM_Pulse = 2;
  TIM_OCConfig.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC2Init(TIM3, &TIM_OCConfig);     //   
  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  TIM3->EGR |= 0x0001;          //   UG    
  TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Disable);     //  

  TIM_Cmd(TIM3, ENABLE);
}


///////////////////////////////////////////////////////////////////////////////////////////
void tim4_Config()              // Генерация звука
{
  TIM_TimeBaseInitTypeDef TIM_BaseConfig;
  TIM_OCInitTypeDef TIM_OCConfig;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

  TIM_OCStructInit(&TIM_OCConfig);
  TIM_TimeBaseStructInit(&TIM_BaseConfig);

  TIM_BaseConfig.TIM_Prescaler = (uint16_t) (SystemCoreClock / 1000000) - 1;    // Делитель (1 тик = 1 мкс)
  TIM_BaseConfig.TIM_ClockDivision = 0;
  TIM_BaseConfig.TIM_Period = 366;      // 366 мкс - 2.730 кгц
  TIM_BaseConfig.TIM_CounterMode = TIM_CounterMode_Up;  // Отсчет от нуля до TIM_Period

  TIM_OCConfig.TIM_OCMode = TIM_OCMode_PWM1;    // Конфигурируем выход таймера, режим - PWM1
  TIM_OCConfig.TIM_OutputState = TIM_OutputState_Enable;        // Собственно - выход включен
  TIM_OCConfig.TIM_Pulse = TIM_BaseConfig.TIM_Period / 2;       // длительность импульса 20 мкс
  TIM_OCConfig.TIM_OCPolarity = TIM_OCPolarity_High;    // Полярность => пульс - это единица (+3.3V)


  TIM_DeInit(TIM4);             // Де-инициализируем таймер №3

  TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM4, ENABLE);

  TIM_TimeBaseInit(TIM4, &TIM_BaseConfig);
  TIM_OC4Init(TIM4, &TIM_OCConfig);     // Инициализируем второй выход

  TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM4, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);


  TIM4->EGR |= 0x0001;          // Устанавливаем бит UG для принудительного сброса счетчика
  TIM_CCxCmd(TIM4, TIM_Channel_4, TIM_CCx_Disable);     // запретить накачку

  TIM_Cmd(TIM4, ENABLE);
}


///////////////////////////////////////////////////////////////////////////////////////////
void tim9_Config()              //  0.1 секунда
{
  TIM_TimeBaseInitTypeDef TIM_BaseConfig;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);

  TIM_TimeBaseStructInit(&TIM_BaseConfig);

  TIM_BaseConfig.TIM_Prescaler = (uint16_t) (SystemCoreClock / 1000) - 1;       // Делитель (1 тик = 1 мс)
  TIM_BaseConfig.TIM_ClockDivision = 0;
  TIM_BaseConfig.TIM_Period = 100;      // 10 гц
  TIM_BaseConfig.TIM_CounterMode = TIM_CounterMode_Up;  // Отсчет от нуля до TIM_Period

  TIM_TimeBaseInit(TIM9, &TIM_BaseConfig);
  TIM_ARRPreloadConfig(TIM9, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel = TIM9_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_ITConfig(TIM9, TIM_IT_Update, ENABLE);


  TIM9->EGR |= 0x0001;          // Устанавливаем бит UG для принудительного сброса счетчика

  TIM_Cmd(TIM9, ENABLE);
}

/////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////
void tim10_Config()             //  Обслуживание контроля напряжения
{
  TIM_TimeBaseInitTypeDef TIM_BaseConfig;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);

  TIM_TimeBaseStructInit(&TIM_BaseConfig);

  TIM_BaseConfig.TIM_Prescaler = (uint16_t) (SystemCoreClock / 1500) - 1;       // частота 3000 Гц.
  TIM_BaseConfig.TIM_ClockDivision = 0;
  TIM_BaseConfig.TIM_Period = 1;        // Апдейт 750 Гц
  TIM_BaseConfig.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM10, &TIM_BaseConfig);
  TIM_ARRPreloadConfig(TIM10, ENABLE);

  /////////////////////////////////////////////////////////////////////////////////

  NVIC_InitStructure.NVIC_IRQChannel = TIM10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_ITConfig(TIM10, TIM_IT_Update, ENABLE);

  TIM10->EGR |= 0x0001;         // Устанавливаем бит UG для принудительного сброса счетчика

  TIM_Cmd(TIM10, ENABLE);
}

/////////////////////////////////////////////////////////////////////////////////
