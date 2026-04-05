#ifndef DRIVERS_NVMC_H
#define DRIVERS_NVMC_H

#include <stdint.h>

#define HIGHSCORE_FLASH_PAGE_ADDRESS 0x0003F000 

uint32_t nvmc_read_highscore(uint8_t difficulty);
void nvmc_write_highscore(uint8_t difficulty, uint32_t new_score);

#endif