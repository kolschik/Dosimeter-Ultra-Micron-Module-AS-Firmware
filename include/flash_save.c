#include "stm32l1xx.h"
#include "main.h"


__IO FLASH_Status FLASHStatus_eeprom = FLASH_COMPLETE;
__IO TestStatus MemoryProgramStatus_eeprom = PASSED;


//////////////////////////////////////////////////////////////////////////////////////
void full_erase_flash(void)     // Erase full dataflash
{
  uint32_t j = 0, Address = 0, NbrOfPage = 0, tmp = 0;

  /* Unlock the FLASH Program memory */
  FLASH_Unlock();

  /* Clear all pending flags */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);

  Address = FLASH_START_ADDR;
  NbrOfPage = ((FLASH_END_ADDR - Address) + 1) >> 8;
  /* Erase the FLASH Program memory pages */
  for (j = 0; j < NbrOfPage; j++)
  {
    tmp = Address + (FLASH_PAGE_SIZE * j);
    if(tmp > FLASH_END_ADDR)
    {
      FLASH_Lock();
      return;
    }
    if(tmp < FLASH_START_ADDR)
    {
      FLASH_Lock();
      return;
    }

    FLASHStatus_eeprom = FLASH_ErasePage(tmp);

    if(FLASHStatus_eeprom != FLASH_COMPLETE)
    {
      MemoryProgramStatus_eeprom = FAILED;
    } else
    {
      FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);
    }
  }

  FLASH_Lock();
}

//////////////////////////////////////////////////////////////////////////////////////
void page_erase_flash(uint32_t page)    // Erase 32 elements
{
  uint32_t Address = 0;
  if(FLASH_MAX_PAGE >= page)    // если не за границами диапазона
  {

    /* Unlock the FLASH Program memory */
    FLASH_Unlock();

    /* Clear all pending flags */
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);

    Address = FLASH_START_ADDR + (page * FLASH_PAGE_SIZE);
    /* Erase the FLASH Program memory pages */

    if(Address > FLASH_END_ADDR)
    {
      FLASH_Lock();
      return;
    }
    if(Address < FLASH_START_ADDR)
    {
      FLASH_Lock();
      return;
    }

    FLASHStatus_eeprom = FLASH_ErasePage(Address);

    if(FLASHStatus_eeprom != FLASH_COMPLETE)
    {
      MemoryProgramStatus_eeprom = FAILED;
    } else
    {
      FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);
    }

    FLASH_Lock();
  }
}

//////////////////////////////////////////////////////////////////////////////////////
void flash_write_massive(uint32_t number_of_massive)
{
  uint32_t Address = 0, i;

  if(number_of_massive > 8)
    return;                     // Проверка входящего параметра

  /* Unlock the FLASH Program memory */
  FLASH_Unlock();

  /* Clear all pending flags */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);

////////////////////////////////////////////////////////////////////////
  for (i = 0; i < 64; i++)      // 2048 / 32 = 64HalfPage 
  {
    // Записываем массив в флеш
    Address = FLASH_START_ADDR + (i << 7) + (number_of_massive << (11 + 2));
    /* Write the FLASH Program memory using HalfPage operation */

    if(Address > FLASH_END_ADDR)
    {
      FLASH_Lock();
      return;
    }
    if(Address < FLASH_START_ADDR)
    {
      FLASH_Lock();
      return;
    }

    FLASHStatus_eeprom = FLASH_ProgramHalfPage(Address, SPECTRO_MASSIVE + (i * 32));

    if(FLASHStatus_eeprom == FLASH_COMPLETE)
    {
    } else
    {
      FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);
    }
  }
////////////////////////////////////////////////////////////////////////
  FLASH_Lock();
}

//////////////////////////////////////////////////////////////////////////////////////
void flash_read_massive(uint32_t number_of_massive)
{


  uint32_t Address = FLASH_START_ADDR, i;

  if(number_of_massive > 8)
    return;                     // Проверка входящего параметра


  for (i = 0; i < 2048; i++)
  {
    SPECTRO_MASSIVE[i] = 0;     // Очищаем массив спектра в памяти

    // Читаем массив из флеша в память
    Address = FLASH_START_ADDR + (i << 2) + (number_of_massive << (11 + 2));
    SPECTRO_MASSIVE[i] = (*(__IO uint32_t *) Address);  // ячейка флеша
  }
}

//////////////////////////////////////////////////////////////////////////////////////
