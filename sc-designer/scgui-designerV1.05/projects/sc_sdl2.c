#include "sc_sdl2.h"

#include <stdio.h>
#include <string.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>


#define SC_SDL2_FLASH_FILE_MAX 8U

#ifndef SC_SDL_PIXEL_SCALE_ENABLE
#define SC_SDL_PIXEL_SCALE_ENABLE 0
#endif

#if SC_SDL_PIXEL_SCALE_ENABLE
#define SC_SDL_PIXEL_SCALE 2
#else
#define SC_SDL_PIXEL_SCALE 1
#endif

#define CANVAS_DRAW_WIDTH (SC_SCREEN_WIDTH * SC_SDL_PIXEL_SCALE)
#define CANVAS_DRAW_HEIGHT (SC_SCREEN_HEIGHT * SC_SDL_PIXEL_SCALE)

typedef struct
{
    SDL_RWops *file;
    uint32_t base;
    uint32_t size;
    uint32_t read_count;
} sc_sdl2_flash_file_t;

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *canvas_texture;
    SDL_Thread *tick_thread;
    SDL_atomic_t tick_running;
    bool running;
    SDL_Rect canvas_rect;
} sc_sdl2_platform_t;

static sc_sdl2_flash_file_t g_flash_files[SC_SDL2_FLASH_FILE_MAX];
static sc_sdl2_platform_t g_platform;
static color_t g_framebuffer[SC_SCREEN_WIDTH * SC_SCREEN_HEIGHT];
static int g_exit_code;
/* 帧缓冲是否有新内容（sc_sdl2_lcd_flush 置位）：静态场景不置位，loop 跳过整帧上传。 */
static bool g_need_upload;

static void sc_sdl2_lcd_init(void)
{
    memset(g_framebuffer, 0, sizeof(g_framebuffer));
    g_need_upload = true;
}

void sc_sdl2_hw_init(void)
{
    sc_sdl2_lcd_init();
    sc_spiflash_unmount_all();
}

void sc_sdl2_lcd_flush(uint16_t xs, uint16_t ys, uint16_t w, uint16_t h, color_t *color)
{
    uint16_t row;
    if (xs >= SC_SCREEN_WIDTH || ys >= SC_SCREEN_HEIGHT)
    {
        return;
    }
    if (xs + w > SC_SCREEN_WIDTH)
    {
        w = (uint16_t)(SC_SCREEN_WIDTH - xs);
    }
    if (ys + h > SC_SCREEN_HEIGHT)
    {
        h = (uint16_t)(SC_SCREEN_HEIGHT - ys);
    }
    for (row = 0; row < h; ++row)
    {
        memcpy(&g_framebuffer[(ys + row) * SC_SCREEN_WIDTH + xs],
               &color[row * w], (size_t)w * sizeof(color_t));
    }
    g_need_upload = true;
}

const color_t *sc_sdl2_framebuffer(void)
{
    return g_framebuffer;
}

static SDL_RWops *sc_sdl2_flash_open(const char *file_name)
{
    char *base_path = SDL_GetBasePath();
    if (base_path)
    {
        char path[1024];
        SDL_snprintf(path, sizeof(path), "%s%s", base_path, file_name);
        SDL_free(base_path);
        SDL_RWops *file = SDL_RWFromFile(path, "rb");
        if (file)
        {
            return file;
        }
    }
    SDL_RWops *file = SDL_RWFromFile(file_name, "rb");
    if (file)
    {
        return file;
    }
    {
        char path[1024];
        SDL_snprintf(path, sizeof(path), "build/%s", file_name);
        return SDL_RWFromFile(path, "rb");
    }
}

void sc_spiflash_unmount_all(void)
{
    for (uint8_t i = 0U; i < SC_SDL2_FLASH_FILE_MAX; ++i)
    {
        if (g_flash_files[i].file)
        {
            SDL_RWclose(g_flash_files[i].file);
        }
        g_flash_files[i] = (sc_sdl2_flash_file_t){0};
    }
}

bool sc_spiflash_mount_file(uint32_t base, const char *file_name)
{
    if (!file_name)
    {
        return false;
    }
    SDL_RWops *file = sc_sdl2_flash_open(file_name);
    if (!file)
    {
        return false;
    }
    Sint64 file_size = SDL_RWsize(file);
    if (file_size <= 0 || (uint64_t)file_size > UINT32_MAX ||
        (uint64_t)base + (uint64_t)file_size > (uint64_t)UINT32_MAX + 1U)
    {
        SDL_RWclose(file);
        return false;
    }

    uint8_t free_index = SC_SDL2_FLASH_FILE_MAX;
    uint64_t new_end = (uint64_t)base + (uint64_t)file_size;
    for (uint8_t i = 0U; i < SC_SDL2_FLASH_FILE_MAX; ++i)
    {
        if (!g_flash_files[i].file)
        {
            if (free_index == SC_SDL2_FLASH_FILE_MAX)
            {
                free_index = i;
            }
            continue;
        }
        if (g_flash_files[i].base == base)
        {
            free_index = i;
            continue;
        }
        uint64_t old_end = (uint64_t)g_flash_files[i].base + g_flash_files[i].size;
        if ((uint64_t)base < old_end && (uint64_t)g_flash_files[i].base < new_end)
        {
            SDL_RWclose(file);
            return false;
        }
    }
    if (free_index == SC_SDL2_FLASH_FILE_MAX)
    {
        SDL_RWclose(file);
        return false;
    }
    if (g_flash_files[free_index].file)
    {
        SDL_RWclose(g_flash_files[free_index].file);
    }
    g_flash_files[free_index] = (sc_sdl2_flash_file_t){
        .file = file,
        .base = base,
        .size = (uint32_t)file_size,
    };
    return true;
}

bool sc_spiflash_is_mounted(uint32_t base)
{
    for (uint8_t i = 0U; i < SC_SDL2_FLASH_FILE_MAX; ++i)
    {
        if (g_flash_files[i].file && g_flash_files[i].base == base)
        {
            return true;
        }
    }
    return false;
}

uint32_t sc_spiflash_read_count(uint32_t base)
{
    for (uint8_t i = 0U; i < SC_SDL2_FLASH_FILE_MAX; ++i)
    {
        if (g_flash_files[i].file && g_flash_files[i].base == base)
        {
            return g_flash_files[i].read_count;
        }
    }
    return 0U;
}

void spiflash_read(uint8_t *buf, uint32_t offset, uint32_t size)
{
    if (!buf)
    {
        return;
    }
    memset(buf, 0xFF, size);
    for (uint8_t i = 0U; i < SC_SDL2_FLASH_FILE_MAX; ++i)
    {
        sc_sdl2_flash_file_t *mapping = &g_flash_files[i];
        if (!mapping->file || offset < mapping->base)
        {
            continue;
        }
        uint32_t file_offset = offset - mapping->base;
        if (file_offset > mapping->size || size > mapping->size - file_offset)
        {
            continue;
        }
        if (SDL_RWseek(mapping->file, file_offset, RW_SEEK_SET) >= 0)
        {
            ++mapping->read_count;
            SDL_RWread(mapping->file, buf, 1U, size);
        }
        return;
    }
}

static int sc_sdl2_tick_thread(void *userdata)
{
    (void)userdata;
    while (SDL_AtomicGet(&g_platform.tick_running))
    {
        SDL_Delay(1);
        sc_systick_inc();
    }
    return 0;
}

static uint32_t sc_sdl2_rgb565_to_argb8888(color_t color)
{
    uint8_t r = (uint8_t)(((color >> 11) & 0x1FU) * 255U / 31U);
    uint8_t g = (uint8_t)(((color >> 5) & 0x3FU) * 255U / 63U);
    uint8_t b = (uint8_t)((color & 0x1FU) * 255U / 31U);
    return 0xFF000000U | ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

static int sc_sdl2_platform_init(sc_sdl2_platform_t *platform)
{
    memset(platform, 0, sizeof(*platform));

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return -1;
    }

    platform->window = SDL_CreateWindow("scgui - SDL Simulator",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        CANVAS_DRAW_WIDTH,
                                        CANVAS_DRAW_HEIGHT,
                                        SDL_WINDOW_SHOWN);
    if (!platform->window)
    {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        return -1;
    }

    platform->renderer = SDL_CreateRenderer(platform->window, -1, SDL_RENDERER_ACCELERATED);
    if (!platform->renderer)
    {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        return -1;
    }

    platform->canvas_texture = SDL_CreateTexture(platform->renderer,
                                                 SDL_PIXELFORMAT_ARGB8888,
                                                 SDL_TEXTUREACCESS_STREAMING,
                                                 SC_SCREEN_WIDTH,
                                                 SC_SCREEN_HEIGHT);
    if (!platform->canvas_texture)
    {
        fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
        return -1;
    }

    platform->canvas_rect.x = 0;
    platform->canvas_rect.y = 0;
    platform->canvas_rect.w = CANVAS_DRAW_WIDTH;
    platform->canvas_rect.h = CANVAS_DRAW_HEIGHT;

    SDL_AtomicSet(&platform->tick_running, 1);
    platform->tick_thread = SDL_CreateThread(sc_sdl2_tick_thread, "sc_tick", NULL);
    if (!platform->tick_thread)
    {
        fprintf(stderr, "SDL_CreateThread failed: %s\n", SDL_GetError());
        return -1;
    }

    platform->running = true;
    return 0;
}

static void sc_sdl2_platform_shutdown(sc_sdl2_platform_t *platform)
{
    if (platform->tick_thread)
    {
        SDL_AtomicSet(&platform->tick_running, 0);
        SDL_WaitThread(platform->tick_thread, NULL);
        platform->tick_thread = NULL;
    }
    if (platform->canvas_texture)
    {
        SDL_DestroyTexture(platform->canvas_texture);
    }
    if (platform->renderer)
    {
        SDL_DestroyRenderer(platform->renderer);
    }
    if (platform->window)
    {
        SDL_DestroyWindow(platform->window);
    }
    SDL_Quit();
}

static void sc_sdl2_upload_canvas(sc_sdl2_platform_t *platform, const color_t *framebuffer)
{
    int y;
    void *pixels = NULL;
    int pitch = 0;

    if (SDL_LockTexture(platform->canvas_texture, NULL, &pixels, &pitch) != 0)
    {
        return;
    }

    for (y = 0; y < SC_SCREEN_HEIGHT; ++y)
    {
        uint32_t *row = (uint32_t *)((uint8_t *)pixels + y * pitch);
        int x;
        for (x = 0; x < SC_SCREEN_WIDTH; ++x)
        {
            row[x] = sc_sdl2_rgb565_to_argb8888(framebuffer[y * SC_SCREEN_WIDTH + x]);
        }
    }

    SDL_UnlockTexture(platform->canvas_texture);
}

static void sc_sdl2_end_frame(sc_sdl2_platform_t *platform)
{
    SDL_RenderCopy(platform->renderer, platform->canvas_texture, NULL, &platform->canvas_rect);
    SDL_RenderPresent(platform->renderer);
}

static int sc_sdl2_key_to_cmd(SDL_Keycode key)
{
    if (key >= SDLK_a && key <= SDLK_z)
    {
        return CMD_A + (int)(key - SDLK_a);
    }
    if (key >= SDLK_1 && key <= SDLK_9)
    {
        return CMD_1 + (int)(key - SDLK_1);
    }
    switch (key)
    {
    case SDLK_0:
        return CMD_0;
    case SDLK_LEFT:
        return CMD_LEFT;
    case SDLK_RIGHT:
        return CMD_RIGHT;
    case SDLK_UP:
        return CMD_UP;
    case SDLK_DOWN:
        return CMD_DOWN;
    case SDLK_DELETE:
        return CMD_DEL;
    case SDLK_RETURN:
        return CMD_ENTER;
    case SDLK_BACKSPACE:
        return CMD_BACK;
    default:
        return CMD_NONE;
    }
}

static void sc_sdl2_post_touch(const sc_sdl2_platform_t *platform, uint8_t type, int x, int y)
{
    sc_event_t translated;
    if (x < platform->canvas_rect.x || y < platform->canvas_rect.y ||
        x >= platform->canvas_rect.x + platform->canvas_rect.w ||
        y >= platform->canvas_rect.y + platform->canvas_rect.h)
    {
        return;
    }
    translated.type = type;
    translated.dat.pos[0] = (int16_t)((x - platform->canvas_rect.x) / SC_SDL_PIXEL_SCALE);
    translated.dat.pos[1] = (int16_t)((y - platform->canvas_rect.y) / SC_SDL_PIXEL_SCALE);
    sc_event_post(&translated);
}

static void sc_sdl2_handle_event(sc_sdl2_platform_t *platform, const SDL_Event *event)
{
    sc_event_t translated;
    int key;

    memset(&translated, 0, sizeof(translated));

    switch (event->type)
    {
    case SDL_QUIT:
        platform->running = false;
        return;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
        key = sc_sdl2_key_to_cmd(event->key.keysym.sym);
        if (key != CMD_NONE)
        {
            translated.type = (event->type == SDL_KEYDOWN) ? SC_EVENT_KEY_DOWN : SC_EVENT_KEY_UP;
            translated.dat.key = key;
        }
        break;
    case SDL_MOUSEBUTTONDOWN:
        sc_sdl2_post_touch(platform, SC_EVENT_TOUCH_DOWN, event->button.x, event->button.y);
        return;
    case SDL_MOUSEBUTTONUP:
        sc_sdl2_post_touch(platform, SC_EVENT_TOUCH_UP, event->button.x, event->button.y);
        return;
    case SDL_MOUSEMOTION:
        if ((event->motion.state & SDL_BUTTON_LMASK) != 0U)
        {
            sc_sdl2_post_touch(platform, SC_EVENT_TOUCH_MOVE, event->motion.x, event->motion.y);
        }
        return;
    default:
        break;
    }

    if (translated.type != SC_EVENT_NONE)
    {
        sc_event_post(&translated);
    }
}

void sc_sdl2_quit(int exit_code)
{
    SDL_Event event;
    g_exit_code = exit_code;
    memset(&event, 0, sizeof(event));
    event.type = SDL_QUIT;
    SDL_PushEvent(&event);
}

int sc_sdl2_exit_code(void)
{
    return g_exit_code;
}

int sc_sdl2_init(void)
{
    g_exit_code = 0;
    sc_sdl2_hw_init();
    if (sc_sdl2_platform_init(&g_platform) != 0)
    {
        sc_sdl2_platform_shutdown(&g_platform);
        return -1;
    }
    return 0;
}

bool sc_sdl2_loop(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        sc_sdl2_handle_event(&g_platform, &event);
    }
    /* 画布纹理铺满全窗口（canvas_rect = CANVAS_DRAW_SIZE），无需 RenderClear 清底 */
    /* 只有帧缓冲有新内容（lcd_flush 置位）才上传 + Present；静态场景整段跳过 */
    if (g_need_upload)
    {
        sc_sdl2_upload_canvas(&g_platform, sc_sdl2_framebuffer());
        sc_sdl2_end_frame(&g_platform);
        g_need_upload = false;
    }
    SDL_Delay(1);
    return g_platform.running;
}

void sc_sdl2_close(void)
{
    sc_sdl2_platform_shutdown(&g_platform);
    memset(&g_platform, 0, sizeof(g_platform));
}
