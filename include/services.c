#include <stdio.h>
#include <stdint.h>
#include "STM32L1xx.h"          // Device header
#include "main.h"
#include "math.h"

#define DOR_OFFSET                 ((uint32_t)0x0000002C)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Индикация

void LED_show(uint8_t digit, uint8_t segment, FunctionalState state)
{
  switch (segment)
  {
  case C_SEG_A:
    if(state == TRUE)
    {
      GPIO_SetBits(GPIOA, GPIO_Pin_3);  // C_SEG_A
    } else
    {
      GPIO_ResetBits(GPIOA, GPIO_Pin_3);
    }
    break;


  case C_SEG_B:
    if(state == TRUE)
    {
      GPIO_SetBits(GPIOA, GPIO_Pin_7);  // C_SEG_B
    } else
    {
      GPIO_ResetBits(GPIOA, GPIO_Pin_7);
    }
    break;


  case C_SEG_C:
    if(state == TRUE)
    {
      GPIO_SetBits(GPIOB, GPIO_Pin_0);  // C_SEG_C
    } else
    {
      GPIO_ResetBits(GPIOB, GPIO_Pin_0);
    }
    break;


  case C_SEG_D:
    if(state == TRUE)
    {
      GPIO_SetBits(GPIOA, GPIO_Pin_6);  // C_SEG_D
    } else
    {
      GPIO_ResetBits(GPIOA, GPIO_Pin_6);
    }
    break;


  case C_SEG_E:
    if(state == TRUE)
    {
      GPIO_SetBits(GPIOA, GPIO_Pin_2);  // C_SEG_E
    } else
    {
      GPIO_ResetBits(GPIOA, GPIO_Pin_2);
    }
    break;


  case C_SEG_F:
    if(state == TRUE)
    {
      GPIO_SetBits(GPIOB, GPIO_Pin_1);  // C_SEG_F
    } else
    {
      GPIO_ResetBits(GPIOB, GPIO_Pin_1);
    }

    break;


  case C_SEG_G:
    if(state == TRUE)
    {
      GPIO_SetBits(GPIOB, GPIO_Pin_12); // C_SEG_G
    } else
    {
      GPIO_ResetBits(GPIOB, GPIO_Pin_12);
    }
    break;


  case C_SEG_ALLOFF:
    GPIO_ResetBits(GPIOA, GPIO_Pin_3);
    GPIO_ResetBits(GPIOA, GPIO_Pin_7);
    GPIO_ResetBits(GPIOB, GPIO_Pin_0);
    GPIO_ResetBits(GPIOA, GPIO_Pin_6);
    GPIO_ResetBits(GPIOA, GPIO_Pin_2);
    GPIO_ResetBits(GPIOB, GPIO_Pin_1);
    GPIO_ResetBits(GPIOB, GPIO_Pin_12);
    break;
  }

  switch (digit)
  {
  case 1:
    if(segment == C_SEG_ALLOFF)
    {
      GPIO_SetBits(GPIOB, GPIO_Pin_2);
    } else
    {
      GPIO_ResetBits(GPIOB, GPIO_Pin_2);
    }
    break;

  case 2:
    if(segment == C_SEG_ALLOFF)
    {
      GPIO_SetBits(GPIOB, GPIO_Pin_10);
    } else
    {
      GPIO_ResetBits(GPIOB, GPIO_Pin_10);
    }
    break;

  case 3:
    if(segment == C_SEG_ALLOFF)
    {
      GPIO_SetBits(GPIOB, GPIO_Pin_11);
    } else
    {
      GPIO_ResetBits(GPIOB, GPIO_Pin_11);
    }
    break;
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
  case 0x30:

    //GPIO_SetBits(GPIOA, GPIO_Pin_3);    // C_SEG_A
    //GPIO_SetBits(GPIOA, GPIO_Pin_7);    // C_SEG_B
    //GPIO_SetBits(GPIOB, GPIO_Pin_0);    // C_SEG_C
    //GPIO_SetBits(GPIOA, GPIO_Pin_6);    // C_SEG_D
    //GPIO_SetBits(GPIOA, GPIO_Pin_2);    // C_SEG_E
    //GPIO_SetBits(GPIOB, GPIO_Pin_1);    // C_SEG_F
    //GPIO_SetBits(GPIOB, GPIO_Pin_12);   // C_SEG_G




    LED_show(LED_show_massive[0], C_SEG_A, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_B, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_C, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_D, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_E, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_F, ENABLE);
    break;


  case 0x31:
    LED_show(LED_show_massive[0], C_SEG_B, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_C, ENABLE);
    break;


  case 0x32:
    LED_show(LED_show_massive[0], C_SEG_A, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_B, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_G, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_E, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_D, ENABLE);
    break;


  case 0x33:
    LED_show(LED_show_massive[0], C_SEG_A, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_B, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_G, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_C, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_D, ENABLE);
    break;


  case 0x34:
    LED_show(LED_show_massive[0], C_SEG_F, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_G, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_B, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_C, ENABLE);
    break;


  case 0x35:
    LED_show(LED_show_massive[0], C_SEG_A, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_F, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_G, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_C, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_D, ENABLE);
    break;


  case 0x36:
    LED_show(LED_show_massive[0], C_SEG_A, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_C, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_D, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_E, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_F, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_G, ENABLE);
    break;


  case 0x37:
    LED_show(LED_show_massive[0], C_SEG_A, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_B, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_C, ENABLE);
    break;


  case 0x38:
    LED_show(LED_show_massive[0], C_SEG_A, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_B, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_C, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_D, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_E, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_F, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_G, ENABLE);
    break;


  case 0x39:
    LED_show(LED_show_massive[0], C_SEG_A, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_B, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_C, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_D, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_F, ENABLE);
    LED_show(LED_show_massive[0], C_SEG_G, ENABLE);
    break;
  }
}
