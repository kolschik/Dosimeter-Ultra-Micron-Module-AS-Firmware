#include "main.h"


//************************************************************************************************************
void adc_init(void)
{
  ADC_InitTypeDef ADC_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;

  /*------------------------ DMA1 configuration ------------------------------*/
  /* Enable DMA1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  /* DMA1 channel1 configuration */
  DMA_DeInit(DMA1_Channel1);
  DMA_StructInit(&DMA_InitStructure);

  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_ADDRESS;   // ����� ���������
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) & ADC_ConvertedValue;       // ����� ������� � ������
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;    // ����������� �������� ������ "��������� -> ������"
  DMA_InitStructure.DMA_BufferSize = 384;       // ���-�� ������ �������� ������ (�.�. ����� ������ �������)
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;      // ���������� ���������� ��������� ���������
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;       // ���������� ���������� ��������� � ������
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;   // ������ ������ ��������� 16 ���
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;   // ������ ���������� � ������ 16 ���
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;       // ����������� ��������
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;   // ������� ���������� ����������
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  // ����� �������� ������-������ ��������
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);

/////////////////////////////////////////////////////////


  RCC_HSICmd(ENABLE);           // �������� HSI
  while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);  // ���� ���� ����������� HSI

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);  // ��������� ������������ ADC

  ADC_BankSelection(ADC1, ADC_Bank_A);

  ADC->CCR |= ADC_CCR_TSVREFE;  //���. ������ ����������� � ����� Vrefint(Tstart=10us)

  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;        // ���������� ��� 12 ���.
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Falling;        // ������ ������������ ������ ���������
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;        // ��������� ����� ����������
  ADC_InitStructure.ADC_NbrOfConversion = 3;    // ����� ��������������
  ADC_Init(ADC1, &ADC_InitStructure);

  ADC_DelaySelectionConfig(ADC1, ADC_DelayLength_Freeze);       // �������� �� ������� ������ ������ �� ���

  ADC_PowerDownCmd(ADC1, ADC_PowerDown_Idle_Delay, DISABLE);    // ���������� ������� ��� � ���������� Idle � Delay

  ADC_TempSensorVrefintCmd(ENABLE);

  while (PWR_GetFlagStatus(PWR_FLAG_VREFINTRDY) == DISABLE);



  //ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 1, ADC_SampleTime_384Cycles);  // ���������������� ������
  /* ADCx Regular Channel Configuration */
  ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 1, ADC_SampleTime_384Cycles);  // ������� ����������
  ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 2, ADC_SampleTime_384Cycles);  // �����������
  ADC_RegularChannelConfig(ADC1, ADC_Channel_19, 3, ADC_SampleTime_384Cycles);  // ���������� ���

  ADC_DelaySelectionConfig(ADC1, ADC_DelayLength_Freeze);       // �������� �� ������� ������ ������ �� ���

  NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure);


  ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);

  ADC_InjectedSequencerLengthConfig(ADC1, 0);
  ADC_InjectedChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_4Cycles);
  ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_Ext_IT15);
  ADC_ExternalTrigInjectedConvEdgeConfig(ADC1, ADC_ExternalTrigInjecConvEdge_Rising);
  //ADC_AutoInjectedConvCmd(ADC1, ENABLE);

  DMA_Cmd(DMA1_Channel1, ENABLE);
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
  ADC_DMACmd(ADC1, ENABLE);


  ADC_Cmd(ADC1, ENABLE);        // ���!
  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET);     // ���� ���� ������� ���
}

//************************************************************************************************************
