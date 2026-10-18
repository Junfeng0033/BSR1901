#ifndef SC_SDL2_H
#define SC_SDL2_H

#include <stdbool.h>
#include <stdint.h>

#include "sc_gui.h"

/* Map a flash address range to a host .bin file (SDL simulator only). */
bool sc_spiflash_mount_file(uint32_t base, const char *file_name);
void sc_spiflash_unmount_all(void);
bool sc_spiflash_is_mounted(uint32_t base);
uint32_t sc_spiflash_read_count(uint32_t base);

/* Simulator hardware: SPI flash map + LCD framebuffer (no SDL window). */
void sc_sdl2_hw_init(void);

/* Simulator LCD flush callback for sc_gui_init(). */
void sc_sdl2_lcd_flush(uint16_t xs, uint16_t ys, uint16_t w, uint16_t h, color_t *color);
const color_t *sc_sdl2_framebuffer(void);

/* Init hardware + SDL window/input + 1 ms tick thread. */
int sc_sdl2_init(void);
/* Poll events and present one frame; returns false after quit. */
bool sc_sdl2_loop(void);
void sc_sdl2_close(void);
/* Post quit event (e.g. after smoke test); optional process exit code. */
void sc_sdl2_quit(int exit_code);
int sc_sdl2_exit_code(void);

#endif /* SC_SDL2_H */
