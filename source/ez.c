#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <nds.h>
#include "main.h"

static void ez_set_rom_page(u16 page) {
	*((vu16*)0x9FE0000) = 0xD200;
	*((vu16*)0x8000000) = 0x1500;
	*((vu16*)0x8020000) = 0xD200;
	*((vu16*)0x8040000) = 0x1500;
	*((vu16*)0x9880000) = page;
	*((vu16*)0x9FC0000) = 0x1500;
}

void dump_ez_nor(int fw_mbit, int data_mbit, void *nor_address, const char *dir_suffix) {
	FILE *file;
	char dir_name[64];
	char file_name[96];
	if (!find_next_dir(dir_name, dir_suffix)) {
		show_message(true, "Could not find directory!");
		return;
	}

	REG_EXMEMCNT = EXMEMCNT_SRAM_TIME_18_CYCLES | EXMEMCNT_ROM_TIME1_8_CYCLES | EXMEMCNT_ROM_TIME2_4_CYCLES | EXMEMCNT_PHI_CLOCK_OFF;
	ez_set_rom_page(0x8002);

	if (fw_mbit > 0) {
		sprintf(file_name, "%s/%s", dir_name, "firmware.bin");
		file = fopen(file_name, "wb");
		if (!file) {
			show_message(true, "Could not open firmware.bin!");
			return;
		}
		printf("Writing firmware.bin...");
		if (!fwrite((void*) 0x08000000, fw_mbit * 131072, 1, file)) {
			show_message(true, "failed");
			return;
		}
		fclose(file);
		printf(" done\n");
	}

	if (data_mbit > 0) {
		sprintf(file_name, "%s/%s", dir_name, "nor.bin");
		file = fopen(file_name, "wb");
		if (!file) {
			show_message(true, "Could not open nor.bin!");
			return;
		}
		printf("Writing nor.bin...");
		for (int i = 0; i < data_mbit / 64; i++) {
			ez_set_rom_page(0x8002 + (i << 12));
			if (!fwrite(nor_address, 64 * 131072, 1, file)) {
				show_message(true, "failed");
				return;
			}
			printf(".");
		}
		fclose(file);
		printf(" done\n");
	}

	show_message(false, "Dumping complete!");
}

void dump_ez3in1_nor(int data_mbit, void *nor_address, const char *dir_suffix) {
	FILE *file;
	char dir_name[64];
	char file_name[96];
	if (!find_next_dir(dir_name, dir_suffix)) {
		show_message(true, "Could not find directory!");
		return;
	}

	REG_EXMEMCNT = EXMEMCNT_SRAM_TIME_18_CYCLES | EXMEMCNT_ROM_TIME1_8_CYCLES | EXMEMCNT_ROM_TIME2_4_CYCLES | EXMEMCNT_PHI_CLOCK_OFF;
	ez_set_rom_page(0);

	if (data_mbit > 0) {
		sprintf(file_name, "%s/%s", dir_name, "nor.bin");
		file = fopen(file_name, "wb");
		if (!file) {
			show_message(true, "Could not open nor.bin!");
			return;
		}
		printf("Writing nor.bin...");
		for (int i = 0; i < data_mbit / 256; i++) {
			if (!fwrite(nor_address, 256 * 131072, 1, file)) {
				show_message(true, "failed");
				return;
			}
			printf(".");
			ez_set_rom_page(768);
		}
		fclose(file);
		printf(" done\n");
	}

	show_message(false, "Dumping complete!");
}
