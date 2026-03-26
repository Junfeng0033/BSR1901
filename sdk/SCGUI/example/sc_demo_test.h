#ifndef SC_DEMO_TEST_H
#define SC_DEMO_TEST_H

#include "sc_gui.h"
#include "sc_menu.h"


extern const sc_image_t tempC_img_48; 
extern const sc_image_t EDA_img_32;      // 图像声明
extern const SC_img_zip logo_160_80_zip; // 压缩图片
extern const SC_img_zip watch_106_64_zip; // 表盘底图

///演示代码性能测试
void sc_demo_rect_pfs(int spi_clk);

void sc_demo_Image_zip(void);


///演示代码,文本显示
void sc_demo_text(void);

//演示代码,组合控件显示
void sc_demo_commpose(void);

//演示代码,按钮显示
void sc_demo_gif_task(sc_event_t *event);
/// 示例图像旋转缩放任务
void sc_demo_trans_task(sc_event_t *event);

/// 示例编辑框任务
void sc_demo_edit_task(sc_event_t *event);

/// 示例脏矩形驱动任务
void sc_demo_drity_task(sc_event_t *event);

///// 示例图表任务
void sc_demo_chart_task(sc_event_t *event);

/// 示例菜单任务
void sc_demo_menu_task(sc_event_t *event);

/// 示例图像任务
void sc_demo_DrawEye_task(sc_event_t *event);

// 示例图像任务
void sc_watch_demo_tast(sc_event_t *event);
#endif 
// SC_DEMO_TEST_H
