#include "main.h"

void dac_init()
{
  DAC_InitTypeDef DAC_InitStructure;
  //--------------------------------------------------------------------
  //Конфигурируем систему детектирования выброса
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

  DAC_StructInit(&DAC_InitStructure);
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_Software;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;

  DAC_Init(DAC_Channel_2, &DAC_InitStructure);
  DAC_Cmd(DAC_Channel_2, ENABLE);
  DAC_SetChannel2Data(DAC_Align_12b_R, 10);     // Set DAC Channel2 DHR register: DAC_OUT2 = (1.224 * 128) / 256 = 0.612 V 

}

void dac_on()
{
  dac_init();
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
  DAC_Cmd(DAC_Channel_2, ENABLE);
}

void dac_off()
{
  DAC_Cmd(DAC_Channel_2, DISABLE);
  DAC_DeInit();
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, DISABLE);
}


void dac_reload()
{
  float voltage;

  voltage = ((float) Settings.feu_voltage / 5005.1) * 5.1;      // 780 вольт = 794.78 мВ
  ADCData.DAC_voltage_raw = voltage / ADCData.Calibration_bit_voltage;  // коррекция значения по напряжению опоры
  // 0,0007326В на 1 бит
  // ADCData.DAC_voltage_raw должно быть примерно равно 1084, при 780 вольтах.

  if((ADCData.DAC_voltage_raw > (DAC_GetDataOutputValue(DAC_Channel_2) + 1))    // вилка по напряжению +-1 бит
     || (ADCData.DAC_voltage_raw < (DAC_GetDataOutputValue(DAC_Channel_2) - 1)))
  {
    dac_off();
    dac_on();
    DAC_SetChannel2Data(DAC_Align_12b_R, ADCData.DAC_voltage_raw);      // Set DAC Channel2 DHR register: DAC_OUT2 = (1.224 * 128) / 256 = 0.612 V 
  }
  DAC_SoftwareTriggerCmd(DAC_Channel_2, ENABLE);
}
