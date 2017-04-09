#ifndef __flashsave_user_H
#define __flashsave_user_H

void full_erase_flash(void);    // Erase full dataflash
void page_erase_flash(uint32_t);        // Erase 16 elements
void flash_write_massive(uint32_t);
void flash_read_massive(uint32_t);

#endif
