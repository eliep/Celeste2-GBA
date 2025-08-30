#include "sram.h"

#ifdef FLASH_RAM

void sram_erase_sector(volatile u8* sector)
{
    volatile vu8* commands = CARTRIDGE_RAM;
    *((vu8*)(commands + 0x5555)) = 0xAA;
    *((vu8*)(commands + 0x2AAA)) = 0x55;
    *((vu8*)(commands + 0x5555)) = 0x80;
    *((vu8*)(commands + 0x5555)) = 0xAA;
    *((vu8*)(commands + 0x2AAA)) = 0x55;
    *sector = 0x30;
     __asm("nop");
     while (*sector != 0xFF)
         ;
}

#endif

int sram_load(SaveData* output)
{
    // NOTE: if you try to read/write sram from a base pointer + offset, the
    // compiler is not guaranteed to emit the correct asm instructions for
    // interacting with the save memory. You should put the target cartridge
    // address in a local variable.
    volatile u8* save_mem = CARTRIDGE_RAM;

    for (int i = 0; i < sizeof *output; ++i)
    {
        ((u8*)output)[i] = *save_mem++;
    }

    return (output->magic == SAVE_DATA_MAGIC) ? 1 : 0;
}

void sram_write_byte(SaveData src, vu8* dest, int index) {
    *((vu8*)(dest + 0x5555)) = 0xAA;
    *((vu8*)(dest + 0x2AAA)) = 0x55;
    *((vu8*)(dest + 0x5555)) = 0xA0;
    *((vu8*)(dest + index)) = ((vu8*)&src)[index];
   __asm("nop");
   while (*((vu8*)(dest + index)) != ((vu8*)&src)[index])
       ;
}

int sram_save(SaveData save)
{
	vu8* save_mem = CARTRIDGE_RAM;
#ifdef FLASH_RAM
    *((vu8*)0x09000000) = 0x00 & 0xFFFF;
    sram_erase_sector(save_mem);
#endif
	// Note: cartridge ram has an 8-bit data bus, so save data _must_ be
	// written one byte at a time.
	for (int i = 0; i < sizeof save; ++i)
	{
#ifdef FLASH_RAM
        sram_write_byte(save, save_mem, i);
#else
        *save_mem++ = ((u8*)&save)[i];
#endif
	}

	SaveData checksum;
	if (!sram_load(&checksum))
	{
		return 0;
	}

	for (int i = 0; i < sizeof checksum; ++i)
	{
		if (*((u8*)&checksum) != *((u8*)&save))
		{
			return 0;
		}
	}

	return 1;
}