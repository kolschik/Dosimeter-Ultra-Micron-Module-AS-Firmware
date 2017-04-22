#include <stdio.h>
#include <stdint.h>
#include "STM32L1xx.h"          // Device header
#include "main.h"
#include "math.h"

#define DOR_OFFSET                 ((uint32_t)0x0000002C)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���������� �������

void Power_off(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;

  PowerState.Off_mode = ENABLE;
  PowerState.Sound = DISABLE;
  Need_Ledupdate = DISABLE;

  TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Disable);     // ��������� �������
  TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
  TIM_ITConfig(TIM3, TIM_IT_CC2, DISABLE);

  //��������� ���� ��������
  TIM_Cmd(TIM2, DISABLE);       // ������������ �������
  TIM_DeInit(TIM2);
  TIM_Cmd(TIM3, DISABLE);       // ��������� ��
  TIM_DeInit(TIM3);
  TIM_Cmd(TIM4, DISABLE);       // ������������ �����
  TIM_DeInit(TIM4);
  TIM_Cmd(TIM9, DISABLE);       // ���� 0.1 �������
  TIM_DeInit(TIM9);
  TIM_Cmd(TIM10, DISABLE);      // ������������ �������� ����������
  TIM_DeInit(TIM10);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, DISABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, DISABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, DISABLE);

  // ��������� ���-���-���
  ADC_Cmd(ADC1, DISABLE);
  ADC_DMACmd(ADC1, DISABLE);
  ADC_TempSensorVrefintCmd(DISABLE);
  DMA_Cmd(DMA1_Channel1, DISABLE);

  DAC_Cmd(DAC_Channel_2, DISABLE);

  DAC_DeInit();
  ADC_DeInit(ADC1);
  DMA_DeInit(DMA1_Channel1);

  RCC_HSICmd(DISABLE);

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, DISABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, DISABLE);


  // ��������� �����������
  PumpCompCmd(OFF_COMP);


  // ���������� USB
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, DISABLE);

  // ������������ �� ������������ �� MSI
  set_msi();
  PWR_UltraLowPowerCmd(ENABLE);
  PWR_PVDCmd(DISABLE);

  // ���������������� ������ GPIOA
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;     // ������ � �������
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;     // ������ � �������
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;     // ������ ��� ������-�������
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;    // USB
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;    // USB
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;    // �������� ������
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  EXTI_InitStructure.EXTI_Line = EXTI_Line15;
  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
  EXTI_Init(&EXTI_InitStructure);


  // ���������������� ������ GPIOB
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;     // �� �� ����������
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;     // ������� ������
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;     // ����� �����
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;    // ����� ���
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;    // ����� ���
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_Init(GPIOB, &GPIO_InitStructure);


  // ���������������� ������ GPIOC-GPIOH
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_Init(GPIOH, &GPIO_InitStructure);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOH, DISABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, DISABLE);

  // ���������� SYSCFG
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, DISABLE);

  // ���������� �������
  DBGMCU_Config(DBGMCU_SLEEP | DBGMCU_STANDBY | DBGMCU_STOP, DISABLE);

  // ���������������� �������
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

  //��������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  // �������
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_Init(GPIOB, &GPIO_InitStructure);


  // ���������� ��������� �������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOB, &GPIO_InitStructure);


}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������� �������

void Power_on(void)
{
  PowerState.Off_mode = DISABLE;
  PumpData.Agressive = ENABLE;
  io_init();
  set_pll_for_usb();

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  Settings.feu_voltage = eeprom_read(0x10);
  Settings.ADC_bits = eeprom_read(0x14);
  Settings.Sound = eeprom_read(0x18);
  Settings.LED_intens = eeprom_read(0x1C);
  Settings.T_korr = eeprom_read(0x20);
  Settings.Impulse_dead_time = eeprom_read(0x28);
  Settings.Start_channel = eeprom_read(0x2C);
  Settings.ADC_time = eeprom_read(0x30);

  delay_ms(550);                // ������� ������� ����������� �����

  adc_init();

  dac_init();
  dac_on();

  PumpCompCmd(INIT_COMP);
  PumpCompCmd(ON_COMP);

  tim2_Config();                // ������������ �������
  tim3_Config();                // ��������� ��
  tim4_Config();                // ������������ �����
  tim9_Config();                // ���� 0.1 �������
  tim10_Config();               // ������������ �������� ����������

  EXTI15_Config();              // �������� ������
  EXTI8_Config();               // ������
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����������� ��������� MCP73831

int MCP73831_state_detect(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  uint32_t down, up, i;

  // �������� PuPd_UP
  up = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);

  // �������� PuPd_DOWN
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  for (i = 0; i < 10000; i++);
  down = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);

  // ��������� � ��������� PuPd_UP
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  if((down == 0) && (up == 1))
    return HI_Z_State;

  if((down == 0) && (up == 0))
    return L_State;

  if((down == 1) && (up == 1))
    return H_State;

  return Unknown_State;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���������

void LED_show(uint16_t digit, uint16_t segment)
{

  if(segment != C_SEG_ALLOFF)
  {
    switch (digit)
    {
    case 1:
      GPIOB->BSRRH = C_DIG1;    // �������� ������
      break;

    case 2:
      GPIOB->BSRRH = C_DIG2;    // �������� ������
      break;

    case 3:
      GPIOB->BSRRH = C_DIG3;    // �������� ������
      break;
    }


    switch (segment)
    {
    case C_SEG_A:
      GPIOA->BSRRL = (uint16_t) 0x0008; // �������� C_SEG_A
      break;


    case C_SEG_B:
      GPIOA->BSRRL = (uint16_t) 0x0080; // �������� C_SEG_B
      break;


    case C_SEG_C:
      GPIOB->BSRRL = (uint16_t) 0x0001; // �������� C_SEG_C
      break;


    case C_SEG_D:
      GPIOA->BSRRL = (uint16_t) 0x0040; // �������� C_SEG_D
      break;


    case C_SEG_E:
      GPIOA->BSRRL = (uint16_t) 0x0004; // �������� C_SEG_E
      break;


    case C_SEG_F:
      GPIOB->BSRRL = (uint16_t) 0x0002; // �������� C_SEG_F
      break;


    case C_SEG_G:
      GPIOB->BSRRL = (uint16_t) 0x1000; // �������� C_SEG_G
      break;

    }
  } else
  {
    GPIOA->BSRRH = (uint16_t) 0x00CC;   // ��������� C_SEG_A  C_SEG_B  C_SEG_D  C_SEG_E
    GPIOB->BSRRH = (uint16_t) 0x1003;   // ��������� C_SEG_�  C_SEG_�  C_SEG_G
    GPIOB->BSRRL = (uint16_t) 0x0C04;   // ��������� ������ 1, 2, 3
  }

}

///////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////
void LEDString(void)            //Displays a string at current cursor location
{
  unsigned char i = 0;

  for (i = 1; i <= 3; i++)
    if(LED_BUF[i - 1])
      LED_show_massive[i] = LED_BUF[i - 1];
}

///////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////
void LEDUpdate(void)
{
  LED_show_massive[0]++;
  if(LED_show_massive[0] > 3)
    LED_show_massive[0] = 1;

  switch (LED_show_massive[LED_show_massive[0]])
  {
  case 0x30:                   // ����� 0
    LED_show(LED_show_massive[0], C_SEG_A);
    LED_show(LED_show_massive[0], C_SEG_B);
    LED_show(LED_show_massive[0], C_SEG_C);
    LED_show(LED_show_massive[0], C_SEG_D);
    LED_show(LED_show_massive[0], C_SEG_E);
    LED_show(LED_show_massive[0], C_SEG_F);
    break;


  case 0x31:                   // ����� 1
    LED_show(LED_show_massive[0], C_SEG_B);
    LED_show(LED_show_massive[0], C_SEG_C);
    break;


  case 0x32:                   // ����� 2
    LED_show(LED_show_massive[0], C_SEG_A);
    LED_show(LED_show_massive[0], C_SEG_B);
    LED_show(LED_show_massive[0], C_SEG_G);
    LED_show(LED_show_massive[0], C_SEG_E);
    LED_show(LED_show_massive[0], C_SEG_D);
    break;


  case 0x33:                   // ����� 3
    LED_show(LED_show_massive[0], C_SEG_A);
    LED_show(LED_show_massive[0], C_SEG_B);
    LED_show(LED_show_massive[0], C_SEG_G);
    LED_show(LED_show_massive[0], C_SEG_C);
    LED_show(LED_show_massive[0], C_SEG_D);
    break;


  case 0x34:                   // ����� 4
    LED_show(LED_show_massive[0], C_SEG_F);
    LED_show(LED_show_massive[0], C_SEG_G);
    LED_show(LED_show_massive[0], C_SEG_B);
    LED_show(LED_show_massive[0], C_SEG_C);
    break;


  case 0x35:                   // ����� 5
    LED_show(LED_show_massive[0], C_SEG_A);
    LED_show(LED_show_massive[0], C_SEG_F);
    LED_show(LED_show_massive[0], C_SEG_G);
    LED_show(LED_show_massive[0], C_SEG_C);
    LED_show(LED_show_massive[0], C_SEG_D);
    break;


  case 0x36:                   // ����� 6
    LED_show(LED_show_massive[0], C_SEG_A);
    LED_show(LED_show_massive[0], C_SEG_C);
    LED_show(LED_show_massive[0], C_SEG_D);
    LED_show(LED_show_massive[0], C_SEG_E);
    LED_show(LED_show_massive[0], C_SEG_F);
    LED_show(LED_show_massive[0], C_SEG_G);
    break;


  case 0x37:                   // ����� 7
    LED_show(LED_show_massive[0], C_SEG_A);
    LED_show(LED_show_massive[0], C_SEG_B);
    LED_show(LED_show_massive[0], C_SEG_C);
    break;


  case 0x38:                   // ����� 8
    LED_show(LED_show_massive[0], C_SEG_A);
    LED_show(LED_show_massive[0], C_SEG_B);
    LED_show(LED_show_massive[0], C_SEG_C);
    LED_show(LED_show_massive[0], C_SEG_D);
    LED_show(LED_show_massive[0], C_SEG_E);
    LED_show(LED_show_massive[0], C_SEG_F);
    LED_show(LED_show_massive[0], C_SEG_G);
    break;


  case 0x39:                   // ����� 9
    LED_show(LED_show_massive[0], C_SEG_A);
    LED_show(LED_show_massive[0], C_SEG_B);
    LED_show(LED_show_massive[0], C_SEG_C);
    LED_show(LED_show_massive[0], C_SEG_D);
    LED_show(LED_show_massive[0], C_SEG_F);
    LED_show(LED_show_massive[0], C_SEG_G);
    break;
  }
}
