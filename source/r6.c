#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <nds.h>
#include "main.h"

static uint8_t r6_spi_write(uint8_t ch) {
	REG_AUXSPICNT = CARD_ENABLE | CARD_SPI_ENABLE | CARD_SPI_HOLD | CARD_SPI_BAUD_4MHz;
	REG_AUXSPIDATA = ch;
	eepromWaitBusy();
	ch = REG_AUXSPIDATA;
	REG_AUXSPICNT = CARD_ENABLE;
	return ch;
}

void dump_r6_nor(void) {
	FILE *file;
	char dir_name[64];
	char file_name[96];
	if (!find_next_dir(dir_name, "_R6")) {
		show_message(true, "Could not find directory!");
		return;
	}

	// Unlock cartridge
	r6_spi_write(0xFE);
	r6_spi_write(0xFD);
	r6_spi_write(0xFB);
	r6_spi_write(0xF7);

	uint8_t command[8];
	uint32_t result;

	// Read NOR flash (M29W800AT)
	command[7] = 0xF1; // Read halfword
	command[6] = 0x01; // Target: Flash
	command[1] = 0x00;
	command[0] = 0x00;

	sprintf(file_name, "%s/%s", dir_name, "nor.bin");
	file = fopen(file_name, "wb");
	if (!file) {
		show_message(true, "Could not open nor.bin!");
		return;
	}
	setvbuf(file, NULL, _IOFBF, 32768);
	printf("Writing nor.bin...");
	for (uint32_t i = 0; i < (1048576 >> 1); i++) {
		command[5] = i;
		command[4] = i >> 8;
		command[3] = i >> 16;
		command[2] = i >> 24;

		// With no delay, some commands don't process correctly;
		// this delay is probably too high.
		cardPolledTransfer(CARD_ACTIVATE | CARD_nRESET | CARD_BLK_SIZE(7)
			| CARD_DELAY1(2296) | CARD_DELAY2(24), &result, 4, command);
		if (!fwrite(&result, 2, 1, file)) {
			show_message(true, "failed");
			return;
		}
		if ((i & 0xFFFF) == 0xFFFF)
			printf(".");
	}

	fflush(file);
	fclose(file);
	printf(" done\n");

	show_message(false, "Dumping complete!");
}
