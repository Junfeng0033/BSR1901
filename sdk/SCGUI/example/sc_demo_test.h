#ifndef SC_DEMO_TEST_H
#define SC_DEMO_TEST_H

#include "sc_gui.h"
#include "sc_menu.h"

extern const sc_image_t EDA_img_32;      // 图像声明
extern const SC_img_zip logo_160_80_zip; // 压缩图片


void sc_move_rect(sc_rect_t *p, int x, int y);

///演示代码圆弧显示
void sc_demo_arc(void);

///演示代码,文本显示
void sc_demo_text(void);

//演示代码,组合控件显示
void sc_demo_commpose(void);

/// 示例图像旋转缩放任务
void sc_demo_trans_tesk(sc_event_t *event);

/// 示例编辑框任务
void sc_demo_edit_tesk(sc_event_t *event);

/// 示例脏矩形驱动任务
void sc_demo_drity_tesk(sc_event_t *event);

///// 示例图表任务
void sc_demo_chart_tesk(sc_event_t *event);

/// 示例菜单任务
void sc_demo_menu_task(sc_event_t *event);

/// 示例图像任务
void sc_demo_DrawEye_tesk(sc_event_t *event);

// 示例图像任务
void sc_watch_demo_test(sc_event_t *event);
#endif 
// SC_DEMO_TEST_H
