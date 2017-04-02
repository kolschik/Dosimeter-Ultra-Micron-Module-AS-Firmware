#include "main.h"

void adc_calibration(void)
{
  uint32_t i, x = 0;
  uint32_t mass[100];

  ADC_AutoInjectedConvCmd(ADC1, DISABLE);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 1, ADC_SampleTime_96Cycles);   // Конфигурирование канала

  for (i = 0; i < 100; i++)
  {
    ADC_SoftwareStartConv(ADC1);        // Стартуем преобразование
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);     // Тупо ждем завершения преобразования
    mass[i] = ADC_GetConversionValue(ADC1);
  }

  for (i = 0; i < 100; i++)
  {
    x += mass[i];
  }

  ADCData.Calibration_bit_voltage = ((1224 * 1000 * 100) / x);  // битовое значение соотв. напряжению референса 1.22в, из него вычисляем скольким микровольтам соответствует 1 бит.

  ADC_AutoInjectedConvCmd(ADC1, ENABLE);

}


//************************************************************************************************************
void adc_init(void)
{
  ADC_InitTypeDef ADC_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_HSICmd(ENABLE);           // Включаем HSI
  while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);  // Ждем пока запустатися HSI

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);  // Разрешаем тактирование ADC

  ADC_BankSelection(ADC1, ADC_Bank_A);

  ADC->CCR |= ADC_CCR_TSVREFE;  //Вкл. датчик температуры и канал Vrefint(Tstart=10us)

  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;        // Разрешение АЦП 12 бит.
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;   // период срабатывания старта конверсии
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;        // Орентация битов результата
  ADC_InitStructure.ADC_NbrOfConversion = 1;    // Число преобразований
  ADC_Init(ADC1, &ADC_InitStructure);

  ADC_DelaySelectionConfig(ADC1, ADC_DelayLength_Freeze);       // Задержка до момента чтения данных из АЦП

  ADC_PowerDownCmd(ADC1, ADC_PowerDown_Idle_Delay, DISABLE);    // отключение питания АЦП в интервалах Idle и Delay

  ADC_TempSensorVrefintCmd(ENABLE);

  while (PWR_GetFlagStatus(PWR_FLAG_VREFINTRDY) == DISABLE);
  //ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 1, ADC_SampleTime_384Cycles);  // Конфигурирование канала

  ADC_DelaySelectionConfig(ADC1, ADC_DelayLength_Freeze);       // Задержка до момента чтения данных из АЦП

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
  ADC_AutoInjectedConvCmd(ADC1, ENABLE);

  ADC_Cmd(ADC1, ENABLE);        // ВКЛ!

  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET);     // Тупо ждем запуска АЦП
}

//************************************************************************************************************
void ADC_Batt_Read(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  uint32_t i;

  while (PWR_GetFlagStatus(PWR_FLAG_VREFINTRDY) == DISABLE);
  // Ножка изиерения напряжения АКБ
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;    // Ножка
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;  // Аналоговый режим
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;      // Без подтяжки
  GPIO_Init(GPIOB, &GPIO_InitStructure);        // Загружаем конфигурацию

  // ===============================================================================================  
  //Подключение токосемной цепочки
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;    // Ножка
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);        // Загружаем конфигурацию
  ADC_RegularChannelConfig(ADC1, ADC_Channel_20, 1, ADC_SampleTime_384Cycles);  // Конфигурирование канала

  GPIO_ResetBits(GPIOB, GPIO_Pin_15);   // Подключаем токосемник
  ADCData.Batt_voltage_raw = 0;
  for (i = 0; i < 10; i++)
  {
    ADC_SoftwareStartConv(ADC1);        // Стартуем преобразование
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);     // Тупо ждем завершения преобразования
    ADCData.Batt_voltage_raw += ADC_GetConversionValue(ADC1);
  }
  ADCData.Batt_voltage_raw /= 10;
  // ===============================================================================================  
  // Отключаем токосемную цепь
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_SetBits(GPIOB, GPIO_InitStructure.GPIO_Pin);     // Подключаем токосемник
}
