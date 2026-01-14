

#include "sc_demo_test.h"

//@brief 演示代码圆弧显示
void sc_demo_arc(void)
{
    int16_t cx = SC_SCREEN_WIDTH / 2;
    int16_t cy = SC_SCREEN_HEIGHT / 2;
    sc_arc_t arc1 = {cx, cy, 50, 40, 1};
    sc_draw_Arc(NULL, &arc1, 50, 310, C_GREEN, C_YELLOW, 120);
    sc_draw_Led(NULL, cx, cy, 20, C_RED, 128);
}

//@brief 演示代码,文本显示
void sc_demo_text(void)
{
    sc_rect_t rect = {10, 60, 80, 60};                                       // 对齐区域
    sc_draw_Text(NULL, 10, 10, gui->font, "Hello World", C_RED, gui->bkc);   // 简单文本
    sc_draw_Num(NULL, 10, 30, 40, 20, gui->font, -1234, 100, C_BLUE, C_RED); // 数字显示
    sc_label_t label;
    sc_init_Label(&rect, &label, gui->font, "CENTER", C_RED, gui->bkc, ALIGN_CENTER); // 标签初对齐
    sc_draw_Label(NULL, 0, 0, &label);
    int x = 0;
    int y = 0;
    int w = 30;
    int h = 20;
    sc_parent_align(&rect, &x, &y, w, h, ALIGN_HOR | ALIGN_BOTTOM);       // 父对齐
    sc_draw_Num(NULL, x, y, w, h, gui->font, 56789, 1, C_BLUE, gui->bkc); // 数字显示对齐
    sc_draw_Frame(NULL, rect.x, rect.y, rect.w, rect.h, 1, C_BLUE, 255);  // 画边框
}

//@brief 演示代码,组合控件显示
void sc_demo_commpose(void)
{
    sc_button_t button1;
    sc_init_Button(&button1, gui->font, "but1", C_BLACK);
    sc_draw_Button(NULL, 20, 10, 60, 30, 8, 6, gui->fc, gui->bc, &button1);
    sc_draw_Switch(NULL, 100, 10, 40, 25, gui->fc, gui->bc, true);
    sc_draw_Bar(NULL, 20, 45, 100, 16, 6, 5, gui->fc, gui->bc, 30, 100);
    sc_draw_Slider(NULL, 20, 70, 120, 16, gui->fc, C_GREEN, 50, 100);
}
//@breif 演示代码,波形图表显示
void sc_demo_chart_tesk(sc_event_t *event)
{
    static sc_chart_t chart[2];
    static uint16_t angle0, vol;
    if (event->type == SC_EVENT_TYPE_INIT) // 创键时初始化
    {
    }
    else if (event->type == SC_EVENT_TYPE_TIMER) // 10ms周期
    {
        vol++;
        angle0 = (angle0 + 2) % 360;
        uint16_t adc0 = sc_sin(angle0) >> 9;
        sc_put_Chart_ch(&chart[0], adc0, 128, C_RED);
        sc_put_Chart_ch(&chart[1], vol % 100, 256, C_BLUE);
        sc_draw_Chart(NULL, 10, 10, 220, 220, C_BLUE, 30, 30, chart, 2);
    }
}

//@brief 演示代码,图像旋转缩放
void sc_demo_trans_tesk(sc_event_t *event)
{
    static Transform_t p[2];       // 旋转参数
    static sc_image_t label_image; // 图像缓存
    static int angle = 0;
    if (event->type == SC_EVENT_TYPE_INIT) // 创键时初始化
    {

        int16_t cx = SC_SCREEN_WIDTH / 3;  // X中心
        int16_t cy = SC_SCREEN_HEIGHT / 2; // Y中心
        // 图像旋转缩放
        sc_init_trans(cx, cy, (void *)&EDA_img_32, &p[0]);
        // label旋转缩放
        sc_label_t label;
        sc_init_Label(NULL, &label, gui->font, "12345", gui->fc, gui->bkc, 0);
        sc_init_trans_label(&label, cx * 2, cy, &label_image, &p[1]);
        sc_dec_zip dec;
        //sc_drwa_Image_zip(NULL, 0, 0, &logo_160_80_zip, &dec,-1);
			  sc_drwa_Image_zip(NULL, 0, 0, &logo_160_80_zip, &dec,(unsigned int)-1);
    }
    else if (event->type == SC_EVENT_TYPE_TIMER) // 10ms周期
    {
        angle = (angle + 2) % 360;
        sc_set_trans_angle(100 + angle, 100 + angle, angle, &p[0]);
        sc_draw_trans(NULL, &p[0]);
        sc_set_trans_angle(256, 256, angle, &p[1]);
        sc_draw_trans(NULL, &p[1]);
    }
}

/// 示例编辑框任务
void sc_demo_edit_tesk(sc_event_t *event)
{
    static sc_edit_t edit1;
    static char label_str[64] = "123456\n78901234567";
    static sc_rect_t box = {100, 60, 120, 100};
    if (event->type == SC_EVENT_TYPE_INIT) // 创键时初始化
    {
        sc_init_Edit(&box, &edit1, gui->font, label_str, C_BLACK, ALIGN_HOR, 64);
        sc_draw_Edit(NULL, &edit1);
    }
    else if (event->type == SC_EVENT_TOUCH_DOWN || event->type == SC_EVENT_TYPE_CMD)
    {
        sc_event_Edit(event, &edit1);
    }
}
/// 示例脏矩移动
void sc_move_rect(sc_rect_t *p, int x, int y)
{
    sc_dirty_mark(NULL, p);
    p->x += x;
    p->y += y;
    p->x = (p->x > SC_SCREEN_WIDTH) ? (p->x - (SC_SCREEN_WIDTH + p->w)) : (p->x < -(int)p->w) ? (p->x + (SC_SCREEN_WIDTH + p->w))
                                                                                              : p->x;

    // Y轴折返
    p->y = (p->y > SC_SCREEN_HEIGHT) ? (p->y - (SC_SCREEN_HEIGHT + p->h)) : (p->y < -(int)p->h) ? (p->y + (SC_SCREEN_HEIGHT + p->h))
                                                                                                : p->y;
    sc_dirty_mark(NULL, p);
}
/// 示例脏矩形驱动任务
void sc_demo_drity_tesk(sc_event_t *event)
{
    static int angle = 0;
    static Transform_t Trans; // 旋转参数
    static sc_rect_t rect1;
    static sc_rect_t rect2;
    static sc_rect_t rect3;
    static sc_rect_t rect4;
    static sc_label_t label;

    const sc_rect_t Slider = (sc_rect_t){20, 40, 120, 16}; // 滑块常量
		
    const sc_arc_t arc1 = {150, 100, 60, 40, 1};           // 圆弧常量
    const sc_rect_t arc_rect = (sc_rect_t){arc1.cx - arc1.r, arc1.cy - arc1.r, arc1.r * 2 + 1, arc1.r * 2 + 1};
    if (event->type == SC_EVENT_TYPE_INIT)
    {
        int16_t cx = SC_SCREEN_WIDTH / 2;                   // X中心
        int16_t cy = SC_SCREEN_HEIGHT / 2;                  // Y中心
        sc_init_trans(cx, cy, (void *)&EDA_img_32, &Trans); // 图像旋转缩放

        rect1 = (sc_rect_t){10, 50, 50, 60};
        rect2 = (sc_rect_t){20, 60, 40, 50};
        rect3 = (sc_rect_t){30, 70, 50, 60};
        rect4 = (sc_rect_t){40, 80, 40, 50};
        sc_init_Label(&rect1, &label, gui->font, "drity", C_RED, gui->bkc, ALIGN_CENTER); // 标签初对齐
					sc_dirty_mark(NULL, &arc_rect); // 标记脏矩形
    }
    else if (event->type == SC_EVENT_TYPE_TIMER) // 10ms周期
    {

        //       sc_set_trans_angle(200 + angle, 200 + angle, angle, &Trans); // 设置旋转角度
        //       sc_dirty_mark(NULL, &Trans.drity); // 标记脏矩形
        sc_dirty_mark(NULL, &Slider);   // 标记脏矩形
        //-------移动矩形标记脏矩形----------------
        sc_move_rect(&rect1, 1, 3);
        sc_move_rect(&rect2, -1, 4);
        sc_move_rect(&rect3, 2, 3);
        sc_move_rect(&rect4, -1, 2);
        angle = (angle + 2) % 360;
			
						
    }
    sc_pfb_t tpfb;
    uint8_t dirty_cnt = 0;
    sc_dirty_t *p_dirty = sc_dirty_merge_out(&dirty_cnt); // 获取脏矩形
    for (int i = 0; i < dirty_cnt; i++)
    {
        sc_rect_t dirty = sc_dirty_to_rect(&p_dirty[i]); // 获取脏矩形
        // printf("dirty:(%d %d %d %d) ->cnt %d\n", dirty.x, dirty.y, dirty.w, dirty.h, i); // 调试信息
        sc_pfb_init_slices(&tpfb, &dirty, gui->bkc); // 初始化切片
        do
        {
            // sc_draw_trans(&tpfb, &Trans);
            sc_draw_Fill(&tpfb, rect1.x, rect1.y, rect1.w, rect1.h, C_GREEN, 128);
            sc_draw_Label(&tpfb, 0, 0, &label);
            sc_draw_Fill(&tpfb, rect2.x, rect2.y, rect2.w, rect2.h, C_RED, 200);
            sc_draw_Rounded_rect(&tpfb, rect3.x, rect3.y, rect3.w, rect3.h, 18, 15, gui->fc, gui->bc, 128);
            sc_draw_Rounded_rect(&tpfb, rect4.x, rect4.y, rect4.w, rect4.h, 8, 5, gui->fc, gui->bc, 160);
            sc_draw_Bar(&tpfb, Slider.x, Slider.y, Slider.w, Slider.h, 8, 6, gui->fc, C_GREEN, angle * 100 / 360, 100);
            sc_draw_Arc(&tpfb, &arc1, 50, 310, C_YELLOW, C_RED, 255);

        } while (sc_pfb_next_slice(&tpfb));
    }
}

// 示例菜单任务
void sc_demo_menu_task(sc_event_t *event)
{
    /// 测试
    static sc_menu_t test_menu;
    // 接收所有事件
    switch (event->type)
    {
    case SC_EVENT_TYPE_INIT:
        sc_menu_init(&test_menu, 100, 50, 100, 25);
        sc_menu_loop_key(&test_menu, CMD_ENTER);
        
        break;
    case SC_EVENT_TYPE_CMD:
        sc_menu_loop_key(&test_menu, event->dat.cmd);
        break;
    default:
        break;
    }
}

void sc_demo_DrawEye_tesk(sc_event_t *event)
{
    sc_rect_t rect1 = (sc_rect_t){10, 10, 50, 50};
    //sc_rect_t rect2 = (sc_rect_t){160, 10, 120, 120};
    static int Eye = 0;
    static int stup = 2;

    if (event->type == SC_EVENT_TYPE_INIT)
    {
        DrawEye_Blink_test(NULL, rect1.x, rect1.y, rect1.w, rect1.h, 0, 0, 0);
    }
    else if (event->type == SC_EVENT_TYPE_TIMER) // 10ms周期
    {
        Eye += stup;
        if (Eye < -10 || Eye > 40)
        {
            stup = -stup;
        }
        DrawEye_Blink_test(NULL, rect1.x, rect1.y, rect1.w, rect1.h, Eye, Eye, C_GREEN);
      //  DrawEye_Blink_test(NULL, rect2.x, rect2.y, rect2.w, rect2.h, Eye, Eye, C_WHITE);
    }
}


extern const SC_img_zip watch_106_64_zip; // 表盘底图

static const uint16_t km_prt_tab[] = { // 指针
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,

    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,

    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff};

static const sc_image_t km_prt =
    {
        .map = (const uint8_t *)&km_prt_tab,
        .mask = NULL,
        .w = 3,
        .h = 30,
};

void sc_watch_demo_test(sc_event_t *event)
{

    static Transform_t Trans; // 旋转参数
    static int16_t angle = 80;
    static int16_t stup = 2;
    static sc_dec_zip dec;

    sc_rect_t rect1 = (sc_rect_t){20, 10, watch_106_64_zip.w, watch_106_64_zip.h};
    if (event->type == SC_EVENT_TYPE_INIT)
    {
        int16_t cx = rect1.w / 2 + rect1.x -7;  // X中心
        int16_t cy = rect1.h / 2 + rect1.y+15;  // Y中心
        sc_init_trans(cx, cy, (void *)&km_prt, &Trans); // 图像旋转缩放
        Trans.center_y -= 30;
        sc_dirty_mark(NULL, &rect1); // 标记脏矩形
    }
    else if (event->type == SC_EVENT_TYPE_TIMER) // 10ms周期
    {
        angle += stup;
        if (angle >= 280 || angle <= 80)
            stup = -stup;
        sc_set_trans_angle( 256, 256, angle, &Trans); // 设置旋转角度
        sc_dirty_mark(NULL, &Trans.drity);                           // 标记脏矩形
    
    }
    sc_pfb_t tpfb;
    uint8_t dirty_cnt = 0;
    sc_dirty_t *p_dirty = sc_dirty_merge_out(&dirty_cnt); // 获取脏矩形
    for (int i = 0; i < dirty_cnt; i++)
    {
        sc_rect_t dirty = sc_dirty_to_rect(&p_dirty[i]); // 获取脏矩形
        sc_pfb_init_slices(&tpfb, &dirty, gui->bkc);     // 初始化切片
        do
        {
            sc_drwa_Image_zip(&tpfb, rect1.x, rect1.y, &watch_106_64_zip, &dec, (unsigned int)-1);
            sc_draw_trans(&tpfb, &Trans);
        } while (sc_pfb_next_slice(&tpfb));
    }
}
