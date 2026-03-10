#include "nvmc.h"
#include <drivers/register_access.h>

#define NVMC_BASE       0x4001E000
#define NVMC_READY      (NVMC_BASE + 0x400)
#define NVMC_CONFIG     (NVMC_BASE + 0x504)
#define NVMC_ERASEPAGE  (NVMC_BASE + 0x508)

#define NVMC_CONFIG_REN 0x00
#define NVMC_CONFIG_WEN 0x01
#define NVMC_CONFIG_EEN 0x02

static void nvmc_wait_ready(void) {
  while (register_read(NVMC_READY) == 0) {}
}

uint32_t nvmc_read_highscore(uint8_t difficulty) {
  if (difficulty < 1 || difficulty > 3) return 0;

  uint32_t address = HIGHSCORE_FLASH_PAGE_ADDRESS + ((difficulty - 1) * 4);
  uint32_t val = register_read(address);
  
  if (val == 0xFFFFFFFF) {
    return 0; // Kein Highscore vorhanden
  }
  return val;
}

void nvmc_write_highscore(uint8_t difficulty, uint32_t new_score) {
  if (difficulty < 1 || difficulty > 3) return;

  // Aktuelle highscores auslesen und in einem Array speichern
  uint32_t scores[3];
  for(int i = 0; i < 3; i++) {
     scores[i] = register_read(HIGHSCORE_FLASH_PAGE_ADDRESS + (i * 4));
  }

  // neuer score im Array aktualisieren
  scores[difficulty - 1] = new_score;

  // Gesamte Flash Page gelöscht
  nvmc_wait_ready();
  register_write(NVMC_CONFIG, NVMC_CONFIG_EEN);
  nvmc_wait_ready();
  register_write(NVMC_ERASEPAGE, HIGHSCORE_FLASH_PAGE_ADDRESS);
  nvmc_wait_ready();

  // Alle 3 highscores wieder in die Flash Page schreiben
  register_write(NVMC_CONFIG, NVMC_CONFIG_WEN);
  nvmc_wait_ready();
  
  for(int i = 0; i < 3; i++) {
     register_write(HIGHSCORE_FLASH_PAGE_ADDRESS + (i * 4), scores[i]);
     nvmc_wait_ready();
  }

  // Flash wird wieder in read-only mode gestellt
  register_write(NVMC_CONFIG, NVMC_CONFIG_REN);
  nvmc_wait_ready();
}