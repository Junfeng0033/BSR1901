
//sim_port.c（SCGUI 移植层，一行不改你的库）

#include "sim_port.h"
#include "scgui.h"

// SCGUI 接口：刷新屏幕
void scgui_port_flush(void) {
    // 不需要额外操作，main里自动刷新
}

