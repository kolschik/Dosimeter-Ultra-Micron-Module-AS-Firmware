#include <stdio.h>
#include <stdint.h>
#include "STM32L1xx.h"          // Device header

#include "main.h"

DataUpdateDef DataUpdate;
ADCDataDef ADCData;
SettingsDef Settings;
AlarmDef Alarm;
PowerDef Power;
DataDef Data;
PumpDataDef PumpData;

uint16_t key;                   // ������ ������� ������ [012]

uint16_t Detector_massive[Detector_massive_pointer_max + 1];
uint32_t ram_Doze_massive[doze_length + 1];     // 1 ������ = 10 �����, �� ���������� �����
uint32_t ram_max_fon_massive[doze_length + 1];  // 1 ������ = 10 �����, �� ���������� �����

uint16_t USB_spectro_pointer = 0;

uint16_t USB_maxfon_massive_pointer = 0;
uint16_t USB_doze_massive_pointer = 0;
uint16_t current_pulse_count = 0;
uint16_t eeprom_address = 0;
uint8_t Pump_on_alarm_count = 0;


uint16_t Detector_massive_pointer = 0;
uint32_t Cal_count = 0;
uint32_t Cal_count_time = 0;
FunctionalState pump_on_impulse = DISABLE;
uint32_t Isotop_counts = 0;

FunctionalState poweroff_state = DISABLE;
FunctionalState hidden_menu = DISABLE;
FunctionalState Pump_on_alarm = DISABLE;

FunctionalState spect_impulse = DISABLE;

uint16_t bat_cal_running = 0;

uint32_t Detector_AB_massive[15];       // 1 ������, ����������� �� 4 ���

FunctionalState Sound_key_pressed = DISABLE;



////////////

uint8_t LED_show_massive[4];    // ����� ����������� �� �������
char LED_BUF[6];
uint32_t counter = 0;
uint32_t SPECTRO_MASSIVE[2049];
FunctionalState Need_Ledupdate = DISABLE;
uint32_t IMPULSE_MASSIVE[11];
uint32_t PUMP_MASSIVE[11];
uint32_t ERR_MASSIVE[11];

uint8_t temperature;
uint16_t akb_voltage;
uint32_t feu_voltage = 650;
uint32_t counter_err = 0;
uint32_t counter_pump = 0;
void LEDString(void)            //Displays a string at current cursor location
{
  unsigned char i = 0;

  for (i = 1; i <= 3; i++)
    if(LED_BUF[i - 1])
      LED_show_massive[i] = LED_BUF[i - 1];
}



/////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{
  //  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x3000);
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  //set_msi();
  set_pll_for_usb();
  USB_on();

  io_init();

  adc_init();
  delay_ms(50);
  adc_calibration();

  dac_init();
  dac_on();
  dac_reload();

  PumpCompCmd(INIT_COMP);

  PumpCompCmd(ON_COMP);


  tim2_Config();                // ������������ �������
  tim3_Config();                // ��������� ��
  tim4_Config();                // ���� ������������ �������
  tim9_Config();                // ���� 1 �������

  EXTI15_Config();

  while (TRUE)
  {
    if(COMP_GetOutputLevel(COMP_Selection_COMP2) == COMP_OutputLevel_Low)
    {
      PumpCmd(ENABLE);
    }

    if(Need_Ledupdate == ENABLE)
    {
      Need_Ledupdate = DISABLE;
      LEDUpdate();
    }



    USB_work();

  }

  // ���������� �������
  // DBGMCU_Config(DBGMCU_SLEEP | DBGMCU_STANDBY | DBGMCU_STOP, DISABLE);






/*  set_bor();
  Power.sleep_now = DISABLE;

  DataUpdate.Need_erase_flash = ENABLE;
  Data.main_menu_stat = 1;

  Settings.Geiger_voltage = 360;        // ���������� �� ������� 360 �����
  Settings.Pump_Energy = 350;   // ������� ������� 350 ���
  DataUpdate.current_flash_page = 0;

  io_init();                    // ������������� ������ ��

  eeprom_write_default_settings();      // ��������, �������� �� EEPROM
  eeprom_read_settings(MAIN_MENU);      // ������ �������� �� EEPROM
  eeprom_read_settings(MODUL_MENU);     // ������ �������� �� EEPROM
  reload_active_isotop_time();  // ������������ ������� ����� �� ��������
  Data.screen = 1;
  Data.AB_fon = 0;
  Power.USB_active = DISABLE;
  Power.sleep_time = Settings.Sleep_time;
  Power.Display_active = ENABLE;

  timer10_Config();
  tim2_Config();
  tim3_Config();
  tim4_Config();
//--------------------------------------------------------------------
  RTC_Config();                 // ������������� ���� � ����
//--------------------------------------------------------------------
// ������������� �������
//--------------------------------------------------------------------
  delay_ms(50);                 // ��������� ��������� ����������
  display_on();
  LcdInit();
  LcdClear();
//--------------------------------------------------------------------
  adc_init();
  delay_ms(100);
  adc_calibration();
  delay_ms(10);
//--------------------------------------------------------------------
  EXTI8_Config();
  EXTI9_Config();
  EXTI3_Config();
  EXTI4_Config();
  EXTI6_Config();

  // ������������� �������
  PumpTimerConfig();


  DataUpdate.Need_batt_voltage_update = ENABLE;

  if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6))
    hidden_menu = ENABLE;       // �������� ��������� ������� ����

  sound_activate();
  delay_ms(500);                // ��������� ��������� ����������
  sound_deactivate();

  while (1)
/////////////////////////////////
  {
    //if(DataUpdate.RTC_tick_update==ENABLE)        RTC_tick_processing();
    if(DataUpdate.Need_fon_update == ENABLE)
      geiger_calc_fon();
    if(key > 0)
      keys_proccessing();
    if(DataUpdate.Need_batt_voltage_update)
      adc_check_event();

    ////////////////////////////////////////////////////


    if((Power.sleep_time > 0) && (!Power.Display_active))
      sleep_mode(DISABLE);      // ���� ������� ��� ��������, � ������� ��� ��� �����������, ��������� ���������� � �������� �������

    if(Power.Display_active)
    {
      check_isotop_time();      // �������� ������� ����������
      if(Power.sleep_time == 0 && !Alarm.Alarm_active)
        sleep_mode(ENABLE);     // ������� ��� �������� �� ����, � ������� ��� �������, �� ��������� ��� � �������� ����������
      if(DataUpdate.Need_display_update == ENABLE)
      {
        DataUpdate.Need_display_update = DISABLE;
        LcdClear_massive();
        if(Data.screen == 1)
          main_screen();
        if(Data.screen == 2)
          menu_screen(NORMAL_menu_mode);
        if(Data.screen == 3)
          stat_screen();
      }
///////////////////////////////////////////////////////////////////////////////
    }
    if((!Power.USB_active) && (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)))
    {
      usb_activate(0x0);        // ���� ������� USB ������ ���������� �������� USB
    }

    if(!Power.USB_active)       // ���� USB �� �������, ����� ������� � ���
    {
      if(bat_cal_running > 0)
        Power.sleep_time = Settings.Sleep_time; // � ������ ���������� ��� �� �����

      if((current_pulse_count < 30) && (Data.fon_level < 10000) && (Data.AB_fon < 10000))       // ���� ������� �� �����������, �� ����� ���� � ���
      {
        if(SystemCoreClock > 20000000)  // ���� ������� ���� 20 ���, �������� �������
          set_msi();

        if(!PumpData.Active && !Power.Sound_active && (Settings.AMODUL_mode == 0))
        {
          PWR_FastWakeUpCmd(ENABLE);
          PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI); // ��������� � ���
          PWR_FastWakeUpCmd(DISABLE);
        } else
        {
          //PWR_EnterSleepMode(PWR_Regulator_ON, PWR_SLEEPEntry_WFI);
        }
      } else
      {                         // ���� ��� ����� �������, ����������� ������� �� �� ��������
        if((ADCData.Power_voltage > 2800) && (SystemCoreClock < 20000000))      // ���� ������� ���� 20 ���, ������� �������
          set_pll_for_usb();
//        if(Settings.AMODUL_mode != 0)   // ���� ������� ������, ���������� � ����� SLEEP
//          PWR_EnterSleepMode(PWR_Regulator_ON, PWR_SLEEPEntry_WFI);

      }

    } else
      USB_work();               // ���� USB �������, ����������� �������� ������
  }
/////////////////////////////////////////////////////////////////////////////// 
*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���������

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
