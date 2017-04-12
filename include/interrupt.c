#include "main.h"



// ===============================================================================================  
// детектор фронта
void EXTI15_Config(void)
{
//  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;

  // Описываем ножку
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);   // Подаем тактирование на порт
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  // Режим ножки "вход"
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;    // Номер ножки в порту
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);        // записиваем конфигурацию

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);        // Подаем тактирование на SYSCFG
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15);        // Подключаем EXTI к ножке

  // Описываем EXTI
  EXTI_InitStructure.EXTI_Line = EXTI_Line15;   // Номер EXTI
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;   // Режим прерывания
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;        // Триггер по нарастающему фронту
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;     // Включаем
  EXTI_Init(&EXTI_InitStructure);       // записиваем конфигурацию

}


// =======================================================
// Кнопка
void EXTI8_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);        // Подаем тактирование на SYSCFG
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource8); // Подключаем EXTI к ножке

  // Описываем EXTI
  EXTI_InitStructure.EXTI_Line = EXTI_Line8;    // Номер EXTI
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;   // Режим прерывания
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;       // Триггер по нарастающему фронту
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;     // Включаем
  EXTI_Init(&EXTI_InitStructure);       // записиваем конфигурацию

  // Описываем канал прерывания
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;    // канал
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
