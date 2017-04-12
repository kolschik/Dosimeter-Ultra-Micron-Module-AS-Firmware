#include <stdio.h>
#include <stdint.h>
#include "STM32L1xx.h"          // Device header
#include "main.h"
#include "math.h"

#define DOR_OFFSET                 ((uint32_t)0x0000002C)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����������� ��������� MCP73831

int MCP73831_state_detect(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  uint8_t down, up, i;

  // �������� PuPd_UP
  up = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);

  // �������� PuPd_DOWN
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  for (i = 0; i < 10; i++);
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
