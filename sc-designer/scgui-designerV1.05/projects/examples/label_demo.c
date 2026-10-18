#include "label_demo.h"

#if SC_USER_OBJ

static sc_container_t k_page;
static sc_label_t k_label_left_props;
static sc_label_t k_label_center_props;
static sc_label_t k_label_right_props;
static sc_label_t k_label_xofs_props;
static sc_label_t k_label_tex_props;
static sc_label_t k_label_scroll_props;
static sc_str_t k_str_props = SC_STR_NEW("Text: ABC", C_WHITE, SC_FONT_DEFAULT, 255U);
static sc_texture_t k_label_texture;

#define SCROLL_SPEED 2
#define SCROLL_MS 30

static void sc_label_scroll_timer(sc_delay_t *timer)
{
    sc_obj_t *obj = timer->arg;
    if (!obj) return;
    sc_label_t *props = obj->props;
    props->x_ofs -= SCROLL_SPEED;
    if (props->x_ofs < -(int16_t)obj->view.w)
    {
        props->x_ofs = (int16_t)obj->view.w;
    }
    sc_obj_invalidate(obj);
    sc_reload_delay(timer, SCROLL_MS);
}

void sc_label_demo_init(void)
{
    sc_obj_t *screen = sc_page_create(NULL, &k_page, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, 1U);

    sc_obj_t *label_left = sc_label_create(screen, &k_label_left_props);
    sc_obj_set_rect(label_left, 10, 10, 200, 24, ALIGN_NONE);
    sc_obj_set_text(label_left, "Left: ABC", SC_FONT_DEFAULT);
    sc_obj_set_align(label_left, ALIGN_LEFT | ALIGN_BORDER_VIS);

    sc_obj_t *label_center = sc_label_create(screen, &k_label_center_props);
    sc_obj_set_rect(label_center, 10, 40, 200, 24, ALIGN_NONE);
    sc_obj_set_fg(label_center, C_CYAN);
    sc_obj_set_text(label_center, "Center: ABC", SC_FONT_DEFAULT);
    sc_obj_set_align(label_center, ALIGN_CENTER | ALIGN_BORDER_VIS);

    sc_obj_t *label_right = sc_label_create(screen, &k_label_right_props);
    sc_obj_set_rect(label_right, 10, 70, 200, 24, ALIGN_NONE);
    sc_obj_set_fg(label_right, C_YELLOW);
    sc_obj_set_text(label_right, "Right: ABC", SC_FONT_DEFAULT);
    sc_obj_set_align(label_right, ALIGN_RIGHT | ALIGN_BORDER_VIS);

    sc_obj_t *label_xofs = sc_label_create(screen, &k_label_xofs_props);
    sc_obj_set_rect(label_xofs, 10, 100, 200, 24, ALIGN_NONE);
    sc_obj_set_fg(label_xofs, C_GREEN);
    sc_obj_set_text(label_xofs, "xofs+20", SC_FONT_DEFAULT);
    sc_obj_set_align(label_xofs, ALIGN_CENTER | ALIGN_BORDER_VIS);
    k_label_xofs_props.x_ofs = (int16_t)(k_label_xofs_props.x_ofs + 20);

    /* 简易文本：省内存，无对齐/偏移，直接在 view.x,view.y 绘制 */
    sc_obj_t *str_obj = sc_str_create(screen, &k_str_props);
    sc_obj_set_rect(str_obj, 10, 130, 200, 24, ALIGN_NONE);
    /* 测试：非 const 声明的 text 可用 API 修改 text/fg/opa */
    sc_obj_set_text(str_obj, "Text writable: OK", SC_FONT_DEFAULT);
    sc_obj_set_fg(str_obj, C_ORANGE);

    sc_obj_t *label_tex = sc_label_create(screen, &k_label_tex_props);
    sc_obj_set_rect(label_tex, 10, 160, 200, 24, ALIGN_NONE);
    sc_obj_set_text(label_tex, "Texture", SC_FONT_DEFAULT);
    sc_obj_add_texture(label_tex, &k_label_texture, SC_GRAD_HOR, C_BLUE, C_RED);
    sc_obj_set_align(label_tex, ALIGN_CENTER | ALIGN_BORDER_VIS);

    sc_obj_t *scroll_label = sc_label_create(screen, &k_label_scroll_props);
    sc_obj_set_rect(scroll_label, 10, 190, 300, 24, ALIGN_NONE);
    sc_obj_set_fg(scroll_label, C_MAGENTA);
    sc_obj_set_text(scroll_label, "Scrolling text marquee - 流动文字测试 - ", SC_FONT_DEFAULT);
    sc_obj_set_align(scroll_label, ALIGN_LEFT | ALIGN_BORDER_VIS);
    sc_add_delay(sc_label_scroll_timer, SCROLL_MS, scroll_label);
}

#endif /* SC_USER_OBJ */
