#ifndef SRAM_H
#define SRAM_H

#include <gba_types.h>

#include "gameobjects.h"

typedef volatile u8 vu8;
#define SAVE_DATA_MAGIC 0xAAAA
#define CARTRIDGE_RAM ((vu8*)0x0E000000)

typedef struct
{
    int magic;
    int level_index;
    int frames;
    int seconds;
    int minutes;
    int hours;
    int shake;
    int berry_count;
    int death_count;
    int current_music;
    bool collected[BERRY_COUNT];
} ALIGN(4) SaveData;

#ifdef FLASH_RAM
void sram_erase_sector(volatile u8* sector);
#endif
int sram_load(SaveData* output);
int sram_save(SaveData save);

#endif