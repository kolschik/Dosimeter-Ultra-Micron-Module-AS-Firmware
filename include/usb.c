#include "main.h"

extern __IO uint8_t Receive_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];
extern __IO uint32_t Receive_length;
extern __IO uint32_t length;
extern __IO uint32_t Send_length;
uint8_t Send_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];
uint32_t packet_sent = 1;
uint32_t packet_receive = 1;

// =========================================================================================
uint8_t prepare_data(uint32_t mode, uint16_t * massive_pointer, uint8_t start_key)      // ���������� ������� ������ � ��������
{
  uint8_t data_key = 0;
  uint8_t address_hi = 0;
  uint8_t address_lo = 0;
  uint8_t fon_1_4 = 0;
  uint8_t fon_2_4 = 0;
  uint8_t fon_3_4 = 0;
  uint8_t fon_4_4 = 0;
  uint32_t used_lenght = 0;
  uint32_t tmp = 0;             // ����������� �����
  uint32_t eeprom_data = 0;     // ����������� �����

  while (used_lenght <= (VIRTUAL_COM_PORT_DATA_SIZE - 7))
  {
    if(mode != eeprom_send_data)
    {
      if(*massive_pointer >= 2047)
      {
        *massive_pointer = 2047;
        return used_lenght;
      }

      address_lo = *massive_pointer & 0xff;
      address_hi = (*massive_pointer >> 8) & 0xff;

      // ���� ������ ����� ������
      data_key = start_key;
      tmp = SPECTRO_MASSIVE[*massive_pointer];
      fon_1_4 = tmp & 0xff;
      fon_2_4 = (tmp >> 8) & 0xff;
      fon_3_4 = (tmp >> 16) & 0xff;
      fon_4_4 = (tmp >> 24) & 0xff;
      *massive_pointer = *massive_pointer + 1;

    } else
    {
      data_key = start_key;
      address_lo = eeprom_address & 0xff;
      address_hi = (eeprom_address >> 8) & 0xff;

      eeprom_data = eeprom_read(eeprom_address);
      eeprom_address += 0x04;

      fon_1_4 = eeprom_data & 0xff;
      fon_2_4 = (eeprom_data >> 8) & 0xff;
      fon_3_4 = (eeprom_data >> 16) & 0xff;
      fon_4_4 = (eeprom_data >> 24) & 0xff;
    }
    Send_Buffer[used_lenght] = data_key;        // �������� ����
    Send_Buffer[used_lenght + 1] = address_hi;  // �������� �� ����� 
    Send_Buffer[used_lenght + 2] = address_lo;  // �������� �� ����� 
    Send_Buffer[used_lenght + 3] = fon_4_4;     // �������� �� ����� 
    Send_Buffer[used_lenght + 4] = fon_3_4;     // �������� �� ����� 
    Send_Buffer[used_lenght + 5] = fon_2_4;     // �������� �� ����� 
    Send_Buffer[used_lenght + 6] = fon_1_4;     // �������� �� ����� 

    used_lenght += 7;
  }
  return used_lenght;
}

// =========================================================================================


//-----------------------------------------------------------------------------------------
void USB_work()
{
  uint32_t wait_count, i;
  uint32_t current_rcvd_pointer = 0;

//---------------------------------------------�������� ������------------------------------------
  if(bDeviceState == CONFIGURED)
  {
    CDC_Receive_DATA();

    if(Receive_length != 0)
    {
      current_rcvd_pointer = 0; // ����� �������� ������� ������� ������
      Data.USB_not_active = 0;  // ����� ������� ������������ USB 

      while (Receive_length > current_rcvd_pointer)
      {
        if(current_rcvd_pointer >= VIRTUAL_COM_PORT_DATA_SIZE)
          return;
        switch (Receive_Buffer[current_rcvd_pointer])
        {

        case 0x01:             // �������� ���� �������� (RCV 1 ����)
          Send_Buffer[0] = 0xE5;        // �������� ����
          Send_Buffer[1] = __DATE__[4]; // ����
          Send_Buffer[2] = __DATE__[5]; // ����
          Send_Buffer[3] = __DATE__[0]; // �����
          Send_Buffer[4] = __DATE__[1]; // �����
          Send_Buffer[5] = __DATE__[2]; // �����
          Send_Buffer[6] = __DATE__[10];        // ���
          Send_length = 7;
          current_rcvd_pointer++;
          break;

        case 0x02:             // �������� ������� ������� (RCV 1 ����)
          Send_length = prepare_data(spectro_data, &USB_spectro_pointer, 0x03); // ���������� ������� ������ � ��������
          if(Send_length == 0)
            current_rcvd_pointer++;     // ���� ������ ��������
          break;


        case 0x04:             // ����� ������� ������� (RCV 1 ����)
          for (i = 0; i <= 2047; i++)
            SPECTRO_MASSIVE[i] = 0;
          if(Send_length == 0)
            current_rcvd_pointer++;     // ���� ������ ��������
          break;


        case 0x05:             // �������� �������� ������ (RCV 1 ����)
          Send_Buffer[0] = 0x06;        // �������� ����
          Send_Buffer[1] = temperature; // ����������� ��
          Send_Buffer[2] = feu_voltage & 0xff;  // ���������� ���������
          Send_Buffer[3] = (feu_voltage >> 8) & 0xff;
          Send_Buffer[4] = (feu_voltage >> 16) & 0xff;
          Send_Buffer[5] = akb_voltage & 0xff;  // ���������� ���
          Send_Buffer[6] = (akb_voltage >> 8) & 0xff;
          Send_Buffer[7] = counter & 0xff;      // ����������� ��������� � �������
          Send_Buffer[8] = (counter >> 8) & 0xff;
          Send_Buffer[9] = (counter >> 16) & 0xff;
          Send_Buffer[10] = (counter >> 24) & 0xff;
          Send_Buffer[11] = counter_err & 0xff; // ����������� ������ � �������
          Send_Buffer[12] = (counter_err >> 8) & 0xff;
          Send_Buffer[13] = (counter_err >> 16) & 0xff;
          Send_Buffer[14] = (counter_err >> 24) & 0xff;
          Send_Buffer[15] = counter_pump & 0xff;        // ����������� ������� � �������
          Send_Buffer[16] = (counter_pump >> 8) & 0xff;
          Send_Buffer[17] = (counter_pump >> 16) & 0xff;
          Send_Buffer[18] = (counter_pump >> 24) & 0xff;

          Send_length = 19;
          current_rcvd_pointer++;
          break;

        case 0x07:             // �������� ���������� (RCV 4 �����)
          if((current_rcvd_pointer + 3) <= Receive_length)      // �������� ������ ��������� �������
          {
            feu_voltage = Receive_Buffer[current_rcvd_pointer + 1] & 0xff;
            feu_voltage += (Receive_Buffer[current_rcvd_pointer + 2] << 8) & 0xff;
            feu_voltage += (Receive_Buffer[current_rcvd_pointer + 3] << 16) & 0xff;

            current_rcvd_pointer += 4;
          } else
          {
            current_rcvd_pointer = Receive_length;      // ������� ������ ��� ������ ����, ��������� ����
          }
          break;


        case 0x39:             // ���������� �������� (RCV 1 ����)
          USB_spectro_pointer = 0;
          current_rcvd_pointer++;
          break;


        default:
          current_rcvd_pointer++;
          break;
        }

        if(Send_length > 0)     // ���� ����� �� �������� �����������
        {
          wait_count = 0;
          while ((packet_sent != 1) && (wait_count < 0xFFFF))
            wait_count++;       // ��������� ������� �� ������� �����

          CDC_Send_DATA((unsigned char *) Send_Buffer, Send_length);
          Send_length = 0;
        }
      }
      Receive_length = 0;
    }
  }

/*  if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)) // ���� 5� �� USB �� ��������, �� ��������� ���
  {

    wait_count = 0;
    while ((packet_sent != 1) && (wait_count < 0xFFFF))
      wait_count++;             // ��������� ������� �� ������� �����

    usb_deactivate(0x00);
  }
*/

}





void USB_off()
{
//---------------------------------------------���������� USB------------------------------------
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, DISABLE);
  Power.USB_active = DISABLE;

}


void USB_on()
{
  //---------------------------------------------��������� USB------------------------------------
  Set_System();
  SystemCoreClockUpdate();
  Set_USBClock();
  USB_Interrupts_Config();
  USB_Init();
  SYSCFG->PMC |= (uint32_t) SYSCFG_PMC_USB_PU;  // Connect internal pull-up on USB DP line
  Power.USB_active = ENABLE;
}
