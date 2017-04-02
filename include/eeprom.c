#include "stm32l1xx.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

__IO FLASH_Status FLASHStatus = FLASH_COMPLETE;
__IO TestStatus DataMemoryProgramStatus = PASSED;
extern __IO uint8_t Receive_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];
uint32_t NbrOfPage = 0, j = 0, Address = 0;


void eeprom_write_settings(uint32_t menu_massive)
{

  //if(eeprom_read(VRef_address) != Settings.VRef)
//    eeprom_write(VRef_address, Settings.VRef);

  //Запись параметров в EEPROM
//  if(eeprom_read(Power_comp_address) != Settings.Power_comp)
  //eeprom_write(Power_comp_address, Settings.Power_comp);



}

//**************************************************************************
void eeprom_apply_settings(uint32_t addr)
{

  if(addr == 0xFFFF)
    return;

  // контраст и реверс дисплея
  if((addr == 0x0C) || (addr == 0x08))
  {
//    display_off();
//    delay_ms(200);
//    display_on();
  }
}

//**************************************************************************

void eeprom_read_settings(uint32_t menu_massive)
{

  //Чтение параметров из EEPROM
//  Settings.VRef = eeprom_read(VRef_address);
//  if(Settings.VRef == 0)
//    Settings.VRef = 1224;

//  Settings.Power_comp = eeprom_read(Power_comp_address);
}


///////////////////////////////////////////////////////////////////////////
void eeprom_loading(uint32_t current_rcvd_pointer)      // Запись в EEPROM
{
  uint16_t eeprom_write_address;
  uint32_t eeprom_data;
  if(current_rcvd_pointer + 7 >= VIRTUAL_COM_PORT_DATA_SIZE)
    return;

  eeprom_write_address = (Receive_Buffer[current_rcvd_pointer + 1] & 0xff);
  eeprom_write_address += ((Receive_Buffer[current_rcvd_pointer + 2] & 0xff) << 8);

  eeprom_data = (Receive_Buffer[current_rcvd_pointer + 3] & 0xff);
  eeprom_data += ((Receive_Buffer[current_rcvd_pointer + 4] & 0xff) << 8);
  eeprom_data += ((Receive_Buffer[current_rcvd_pointer + 5] & 0xff) << 16);
  eeprom_data += ((Receive_Buffer[current_rcvd_pointer + 6] & 0xff) << 24);

  if(eeprom_read(eeprom_write_address) != eeprom_data)
  {
    eeprom_write(eeprom_write_address, eeprom_data);
  }

}


//**************************************************************************
void eeprom_erase(void)
{
  DATA_EEPROM_Unlock();         // разблокировка перед записью

  /* Clear all pending flags */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);

  /*  Data EEPROM Fast Word program of FAST_DATA_32 at addresses defined by 
     DATA_EEPROM_START_ADDR and DATA_EEPROM_END_ADDR */
  Address = DATA_EEPROM_START_ADDR;

  NbrOfPage = ((DATA_EEPROM_END_ADDR - Address) + 1) >> 2;

  /* Erase the Data EEPROM Memory pages by Word (32-bit) */
  for (j = 0; j < NbrOfPage; j++)
  {
    FLASHStatus = DATA_EEPROM_EraseWord(Address + (4 * j));
  }
  DATA_EEPROM_Lock();

  /* Check the correctness of written data */
  while (Address < DATA_EEPROM_END_ADDR)
  {
    if(*(__IO uint32_t *) Address != 0x0)
    {
      DataMemoryProgramStatus = FAILED;
      //ОШИБКА стирания !!!!
      return;
    }
    Address = Address + 4;
  }

}



//************************************************************************
uint32_t eeprom_read(uint32_t address_of_read)
{

  /* Clear all pending flags */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);

  /*  Data EEPROM Fast Word program of FAST_DATA_32 at addresses defined by 
     DATA_EEPROM_START_ADDR and DATA_EEPROM_END_ADDR */
  Address = DATA_EEPROM_START_ADDR + address_of_read;
  if(Address > DATA_EEPROM_END_ADDR)
  {
    // ОШИБКА вне диапазона eeprom !!!!
    return 0x00;
  }
  // чтение данных
  return *(__IO uint32_t *) Address;
}



//************************************************************************

void eeprom_write(uint32_t address_of_read, uint32_t data)
{

  DATA_EEPROM_Unlock();         // разблокировка перед записью

  /* Clear all pending flags */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);

  /*  Data EEPROM Fast Word program of FAST_DATA_32 at addresses defined by 
     DATA_EEPROM_START_ADDR and DATA_EEPROM_END_ADDR */
  Address = DATA_EEPROM_START_ADDR + address_of_read;
  if(Address > DATA_EEPROM_END_ADDR)
  {
    // ОШИБКА вне диапазона eeprom !!!!
    DATA_EEPROM_Lock();
    return;
  }
  // Запись данных по адресу
  FLASHStatus = DATA_EEPROM_ProgramWord(Address, data);
  DATA_EEPROM_Lock();

  if(FLASHStatus != FLASH_COMPLETE)
  {
    // ОШИБКА записи eeprom !!!!
    return;
  }
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR);


  // Проверка записанных данных 
  if(*(__IO uint32_t *) Address != data)
  {
    DataMemoryProgramStatus = FAILED;
    // ОШИБКА записи !!!!!
    return;
  }



}
