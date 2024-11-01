#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <nds.h>
#include <fat.h>
#include <dirent.h>
#include <sys/stat.h>

bool dir_exists(const char *path) {
	DIR *dir = opendir(path);
	if (dir) {
		closedir(dir);
		return true;
	} else {
		return false;
	}
}

bool find_next_dir(char *buffer, const char *suffix) {
	for (int i = 1; i < 100000; i++) {
		sprintf(buffer, "%05d%s", i, suffix);
		if (!dir_exists(buffer)) {
			if (mkdir(buffer, 0755) == 0) {
				printf("Dumping to %s/\n", buffer);
				return true;
			}
		}
	}
	return false;
}

void show_message(bool is_error, const char *text) {
	printf("%s%s\x1b[39;0m\n\nPress any key to exit", is_error ? "\x1b[31;1m" : "", text);

	scanKeys();
	while (true) {
		swiWaitForVBlank();
		scanKeys();
		if (keysDown()) break;
	}
}

// ez.c
void dump_ez_nor(int fw_mbit, int data_mbit, void *nor_address, const char *dir_suffix);
void dump_ez3in1_nor(int data_mbit, void *nor_address, const char *dir_suffix);

// r6.c
void dump_r6_nor(void);

const char *main_menu_option_str[] = {
//	 012345678901234567890123456789
	"Dump EZ4 NOR    (32+256 Mbit)",
	"Dump EZ4Lite NOR(32+384 Mbit)",
	"Dump EZ4LC NOR  (32+64 Mbit)",
	"Dump EZ 3in1 NOR(0+256 Mbit)",
	"Dump EZ Omega NOR",
	"Dump MK6/R6 NOR"
};
enum main_menu_option {
	MENU_DUMP_EZ_NOR_32_256,
	MENU_DUMP_EZ_NOR_32_384,
	MENU_DUMP_EZ_NOR_32_64,
	MENU_DUMP_EZ_NOR_0_256,
	MENU_DUMP_EZ_NOR_64_512,
	MENU_DUMP_R6_NOR,
	MAIN_MENU_OPTION_COUNT
};

int main(int argc, char **argv) {
	consoleDemoInit();
	lcdMainOnTop();

	if (!fatInitDefault()) {
		show_message(true, "FAT initialization failed!");
		return 0;
	}

	int menuPos = 0;

	while (true) {
		consoleClear();
		printf("\n        ez-dumper " VERSION "\n\n");

		for (int i = 0; i < MAIN_MENU_OPTION_COUNT; i++) {
			printf("%s%s\n", menuPos == i ? "> " : "  ", main_menu_option_str[i]);
		}

		while (true) {
			swiWaitForVBlank();
			scanKeys();

			if (keysDown() & KEY_UP) {
				if (menuPos > 0) {
					menuPos--;
					break;
				}
			}

			if (keysDown() & KEY_DOWN) {
				if (menuPos < (MAIN_MENU_OPTION_COUNT - 1)) {
					menuPos++;
					break;
				}
			}

			if (keysDown() & KEY_A) {
				printf("\n");
				switch (menuPos) {
//					case MENU_DUMP_EZ_NOR_32_512: dump_ez_nor(32, 512, (void*) 0x9400000, "_EZ4"); break;
					case MENU_DUMP_EZ_NOR_32_384: dump_ez_nor(32, 384, (void*) 0x9400000, "_EZ4"); break;
					case MENU_DUMP_EZ_NOR_32_256: dump_ez_nor(32, 256, (void*) 0x9400000, "_EZ4"); break;
					case MENU_DUMP_EZ_NOR_32_64: dump_ez_nor(32, 64, (void*) 0x9400000, "_EZ4LC"); break;
//					case MENU_DUMP_EZ_NOR_0_512: dump_ez3in1_nor(512, (void*) 0x8000000, "_EZ3IN1"); break;
					case MENU_DUMP_EZ_NOR_0_256: dump_ez3in1_nor(256, (void*) 0x8000000, "_EZ3IN1"); break;
					case MENU_DUMP_EZ_NOR_64_512: dump_ez_nor(64, 512, (void*) 0x9000000, "_EZO"); break;
					case MENU_DUMP_R6_NOR: dump_r6_nor(); break;
				}
				break;
			}
		}
	}

	show_message(false, "");
	return 0;
}
