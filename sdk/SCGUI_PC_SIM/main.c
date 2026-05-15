

/*
scgui_pc_sim/
├─ scgui/         # 你从 gitee 下载的 SCGUI 源码全部放这里
├─ main.c         # SDL2 PC 模拟器入口
├─ sim_port.c     # SCGUI 移植接口
└─ sim_port.h     # 头文件

*/

#include <stdio.h>
#include <SDL.h>
#include "scgui.h"
#include "sim_port.h"

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define SCALE_FACTOR  4  // 屏幕放大倍数，方便看

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* texture = NULL;
static uint8_t frame_buf[SCREEN_WIDTH * SCREEN_HEIGHT / 8];

// 刷新屏幕到 PC 窗口
void sim_flush(uint8_t* buf) {
    SDL_UpdateTexture(texture, NULL, buf, SCREEN_WIDTH / 8);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("SCGUI PC 模拟器",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH * SCALE_FACTOR, SCREEN_HEIGHT * SCALE_FACTOR, 0);

    renderer = SDL_CreateRenderer(window, -1, 0);
    texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_INDEX1MSB,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH, SCREEN_HEIGHT);

    // 初始化 SCGUI
    scgui_init(frame_buf, SCREEN_WIDTH, SCREEN_HEIGHT);
    sim_flush(frame_buf);

    // ------------ 在这里写你的界面代码 ------------
    scgui_clear();
    scgui_string(0, 0, "SCGUI PC OK!");
    scgui_box(10, 20, 50, 30);
    scgui_update();
    // --------------------------------------------

    sim_flush(frame_buf);

    // 等待窗口关闭
    while (1) {
        SDL_Event e;
        if (SDL_PollEvent(&e) && e.type == SDL_QUIT) break;
        SDL_Delay(10);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}



