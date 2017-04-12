#include "main.h"



// ===============================================================================================  
// �������� ������
void EXTI15_Config(void)
{
//  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;

  // ��������� �����
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);   // ������ ������������ �� ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  // ����� ����� "����"
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;    // ����� ����� � �����
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);        // ���������� ������������

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);        // ������ ������������ �� SYSCFG
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15);        // ���������� EXTI � �����

  // ��������� EXTI
  EXTI_InitStructure.EXTI_Line = EXTI_Line15;   // ����� EXTI
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;   // ����� ����������
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;        // ������� �� ������������ ������
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;     // ��������
  EXTI_Init(&EXTI_InitStructure);       // ���������� ������������

}


// =======================================================
// ������
void EXTI8_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);        // ������ ������������ �� SYSCFG
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource8); // ���������� EXTI � �����

  // ��������� EXTI
  EXTI_InitStructure.EXTI_Line = EXTI_Line8;    // ����� EXTI
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;   // ����� ����������
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;       // ������� �� ������������ ������
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;     // ��������
  EXTI_Init(&EXTI_InitStructure);       // ���������� ������������

  // ��������� ����� ����������
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;    // �����
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
