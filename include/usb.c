#include "main.h"

extern __IO uint8_t Receive_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];
extern __IO uint32_t Receive_length;
extern __IO uint32_t length;
extern __IO uint32_t Send_length;
uint8_t Send_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];
uint32_t packet_sent = 1;
uint32_t packet_receive = 1;

// =========================================================================================
uint8_t prepare_data(uint32_t mode, uint16_t * massive_pointer, uint8_t start_key)      // Подготовка массива данных к передаче
{
  uint8_t data_key = 0;
  uint8_t address_hi = 0;
  uint8_t address_lo = 0;
  uint8_t fon_1_4 = 0;
  uint8_t fon_2_4 = 0;
  uint8_t fon_3_4 = 0;
  uint8_t fon_4_4 = 0;
  uint32_t used_lenght = 0;
  uint32_t tmp = 0;             // контрольная сумма

  while (used_lenght <= (VIRTUAL_COM_PORT_DATA_SIZE - 7))
  {
    if(*massive_pointer >= 2047)
    {
      *massive_pointer = 2047;
      return used_lenght;
    }

    address_lo = *massive_pointer & 0xff;
    address_hi = (*massive_pointer >> 8) & 0xff;

    // Если данные сжать нельзя
    data_key = start_key;

    tmp = SPECTRO_MASSIVE[*massive_pointer];
/*
    // Антиалиасинг ячеек кратных 64
    if((*massive_pointer & 0x3F) == 0x3F)
    {
      tmp = (SPECTRO_MASSIVE[*massive_pointer - 1] + SPECTRO_MASSIVE[*massive_pointer + 1]) >> 1;
    }
*/

    fon_1_4 = tmp & 0xff;
    fon_2_4 = (tmp >> 8) & 0xff;
    fon_3_4 = (tmp >> 16) & 0xff;
    fon_4_4 = (tmp >> 24) & 0xff;
    *massive_pointer = *massive_pointer + 1;

    Send_Buffer[used_lenght] = data_key;        // передать ключ
    Send_Buffer[used_lenght + 1] = address_hi;  // передать по УСАПП 
    Send_Buffer[used_lenght + 2] = address_lo;  // передать по УСАПП 
    Send_Buffer[used_lenght + 3] = fon_4_4;     // передать по УСАПП 
    Send_Buffer[used_lenght + 4] = fon_3_4;     // передать по УСАПП 
    Send_Buffer[used_lenght + 5] = fon_2_4;     // передать по УСАПП 
    Send_Buffer[used_lenght + 6] = fon_1_4;     // передать по УСАПП 

    used_lenght += 7;
  }
  return used_lenght;
}

// =========================================================================================


//-----------------------------------------------------------------------------------------
void USB_work()
{
  uint32_t wait_count, i, tmp;
  uint32_t current_rcvd_pointer = 0;

//---------------------------------------------Передача данных------------------------------------
  if(bDeviceState == CONFIGURED)
  {
    CDC_Receive_DATA();

    if(Receive_length != 0)
    {
      current_rcvd_pointer = 0; // сброс счетчика текущей позиции приема

      while (Receive_length > current_rcvd_pointer)
      {
        if(current_rcvd_pointer >= VIRTUAL_COM_PORT_DATA_SIZE)
          return;
        switch (Receive_Buffer[current_rcvd_pointer])
        {

        case 0x01:             // Отправка даты прошивки (RCV 1 байт)
          Send_Buffer[0] = 0xE5;        // передать ключ
          Send_Buffer[1] = __DATE__[4]; // день
          Send_Buffer[2] = __DATE__[5]; // день
          Send_Buffer[3] = __DATE__[0]; // месяц
          Send_Buffer[4] = __DATE__[1]; // месяц
          Send_Buffer[5] = __DATE__[2]; // месяц
          Send_Buffer[6] = __DATE__[10];        // год
          Send_length = 7;
          current_rcvd_pointer++;
          break;

        case 0x02:             // передача массива спектра (RCV 1 байт)
          Send_length = prepare_data(spectro_data, &USB_spectro_pointer, 0x03); // Подготовка массива данных к передаче
          if(Send_length == 0)
            current_rcvd_pointer++;     // Если массив исчерпан
          break;


        case 0x04:             // сброс массива спектра (RCV 1 байт)
          for (i = 0; i <= 2047; i++)
            SPECTRO_MASSIVE[i] = 0;
          spectro_time = 0;
          if(Send_length == 0)
            current_rcvd_pointer++;     // Если массив исчерпан
          break;


        case 0x05:             // Отправка основных данных (RCV 1 байт)


          Send_Buffer[0] = 0x06;        // передать ключ
          Send_Buffer[1] = ADCData.Temp & 0xff; // температура МК
          Send_Buffer[2] = Settings.feu_voltage & 0xff; // напряжение детектора
          Send_Buffer[3] = (Settings.feu_voltage >> 8) & 0xff;
          Send_Buffer[4] = (Settings.feu_voltage >> 16) & 0xff;
          Send_Buffer[5] = akb_voltage & 0xff;  // напряжение АКБ
          Send_Buffer[6] = (akb_voltage >> 8) & 0xff;
          Send_Buffer[7] = counter & 0xff;      // колличество импульсов в секунду
          Send_Buffer[8] = (counter >> 8) & 0xff;
          Send_Buffer[9] = (counter >> 16) & 0xff;
          Send_Buffer[10] = (counter >> 24) & 0xff;

          tmp = spectro_time / 10;
          Send_Buffer[11] = tmp & 0xff; // Время накопления спектра
          Send_Buffer[12] = (tmp >> 8) & 0xff;  // Время накопления спектра
          Send_Buffer[13] = (tmp >> 16) & 0xff; // Время накопления спектра
          Send_Buffer[14] = (tmp >> 24) & 0xff; // Время накопления спектра

          Send_Buffer[15] = counter_pump & 0xff;        // колличество накачек в секунду
          Send_Buffer[16] = (counter_pump >> 8) & 0xff;
          Send_Buffer[17] = (counter_pump >> 16) & 0xff;
          Send_Buffer[18] = (counter_pump >> 24) & 0xff;
          Send_Buffer[19] = eeprom_read(0x100) & 0xff;  // Колличество накопленных спектров
          Send_Buffer[20] = Settings.Sound & 0xff;      // Управление звуком
          Send_Buffer[21] = Settings.LED_intens & 0xff; // Управление интенсовностью подсветки
          Send_Buffer[22] = Settings.T_korr & 0xff;     // Температурная коррекция
          Send_Buffer[23] = 0x00;
          Send_Buffer[24] = Settings.Impulse_dead_time & 0xff;  // колличество накачек в секунду
          Send_Buffer[25] = debug_mode & 0xff;  // колличество накачек в секунду



          Send_length = 26;
          current_rcvd_pointer++;
          break;

        case 0x07:             // Загрузка конфигурации (RCV 10 байт)
          if((current_rcvd_pointer + 10) <= Receive_length)     // Проверка длинны принятого участка
          {

            // Напряжение ФЭУ - 3 бита
            Settings.feu_voltage = Receive_Buffer[current_rcvd_pointer + 1] & 0xff;
            Settings.feu_voltage += (Receive_Buffer[current_rcvd_pointer + 2] & 0xff) << 8;
            Settings.feu_voltage += (Receive_Buffer[current_rcvd_pointer + 3] & 0xff) << 16;
            current_rcvd_pointer += 3;
            eeprom_write(0x10, Settings.feu_voltage);
            dac_reload();

            // Битность АЦП - 1 бит
            Settings.ADC_bits = Receive_Buffer[current_rcvd_pointer + 1] & 0xff;
            current_rcvd_pointer++;
            eeprom_write(0x14, Settings.ADC_bits);

            // Звук - 1 бит
            Settings.Sound = Receive_Buffer[current_rcvd_pointer + 1] & 0xff;
            current_rcvd_pointer++;
            eeprom_write(0x18, Settings.Sound);

            // Яркость LED - 1 бит
            Settings.LED_intens = Receive_Buffer[current_rcvd_pointer + 1] & 0xff;
            current_rcvd_pointer++;
            eeprom_write(0x1C, Settings.LED_intens);
            tim2_Config();

            // Коррекция температуры - 1 бит
            Settings.T_korr = Receive_Buffer[current_rcvd_pointer + 1] & 0xff;
            current_rcvd_pointer++;
            eeprom_write(0x20, Settings.T_korr);

            current_rcvd_pointer++;

            // Мертвое время импульса - 1 бит
            Settings.Impulse_dead_time = Receive_Buffer[current_rcvd_pointer + 1] & 0xff;
            current_rcvd_pointer++;
            eeprom_write(0x28, Settings.Impulse_dead_time);
            TIM_SetCompare1(TIM10, Settings.Impulse_dead_time);

            // Режим отладки - 1 бит
            if((Receive_Buffer[current_rcvd_pointer + 1] & 0xff) > 0)
            {
              debug_mode = ENABLE;
            } else
            {
              debug_mode = DISABLE;
            }

            current_rcvd_pointer++;


            ////////////////////////////////////
            current_rcvd_pointer++;
          } else
          {
            current_rcvd_pointer = Receive_length;      // Принято меньше чем должно быть, завершаем цикл
          }
          break;

        case 0x39:             // завершение передачи (RCV 1 байт)
          USB_spectro_pointer = 0;
          current_rcvd_pointer++;
          break;

        case 0x40:             // Загрузка спектра 0
          for (i = 0; i <= 2047; i++)
            SPECTRO_MASSIVE[i] = 0;
          spectro_time = 0;
          flash_read_massive(0);
          spectro_time = eeprom_read(0x100 + 0x04);     // время спектра
          ADCData.Temp = eeprom_read(0x100 + 0x08);     // запись температуры спектра

          current_rcvd_pointer++;
          break;


        default:
          current_rcvd_pointer++;
          break;
        }

        if(Send_length > 0)     // Если пакет на передачу сформитован
        {
          wait_count = 0;
          while ((packet_sent != 1) && (wait_count < 0xFFFF))
            wait_count++;       // Проверяем передан ли прошлый пакет

          CDC_Send_DATA((unsigned char *) Send_Buffer, Send_length);
          Send_length = 0;
        }
      }
      Receive_length = 0;
    }
  }

/*  if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)) // если 5В на USB не подается, то отключаем его
  {

    wait_count = 0;
    while ((packet_sent != 1) && (wait_count < 0xFFFF))
      wait_count++;             // Проверяем передан ли прошлый пакет

    usb_deactivate(0x00);
  }
*/

}





void USB_off()
{
//---------------------------------------------Отключение USB------------------------------------
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, DISABLE);

  PowerState.USB = DISABLE;
  PowerState.Spectr = DISABLE;
  dac_on();                     // Включение ЦАП
  PumpCompCmd(INIT_COMP);       // Включение компоратора
  PumpCompCmd(ON_COMP);
  TIM_Cmd(TIM10, ENABLE);       // Включение контроля напряжения на ФЭУ

  TIM_Cmd(TIM2, ENABLE);        // Индикацию включить
  TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  LED_show(LED_show_massive[0], C_SEG_ALLOFF);

  PowerState.Sound = ENABLE;    // Звук включить
}


void USB_on()
{
  //---------------------------------------------Включение USB------------------------------------
  PowerState.USB = ENABLE;
  Set_System();
  SystemCoreClockUpdate();
  Set_USBClock();
  USB_Interrupts_Config();
  USB_Init();
  SYSCFG->PMC |= (uint32_t) SYSCFG_PMC_USB_PU;  // Connect internal pull-up on USB DP line
}
