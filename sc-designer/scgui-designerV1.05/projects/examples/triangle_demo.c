#include "triangle_demo.h"

#if SC_USER_OBJ && SC_WIDGET_TRIANGLE_ENABLE

static sc_container_t k_page;
static sc_label_t k_title;
static sc_label_t k_hint;
static sc_triangle_t k_tri_up;
static sc_triangle_t k_tri_down;
static sc_triangle_t k_tri_left;
static sc_triangle_t k_tri_right;
static sc_triangle_t k_tri_rot;

/* 交互三角形的锚点 + 当前朝向。 */
static int16_t s_rot_x;
static int16_t s_rot_y;
static uint8_t s_rot_idx;

/* 40×36 包围盒内的 4 种朝向：[x1,y1,x2,y2,x3,y3]（相对锚点）。 */
#define TRI_ORIENT_N 4U
static const int16_t k_orients[TRI_ORIENT_N][6] = {
    { 20, 0, 4, 36, 36, 36 },   /* 朝上 */
    { 20, 36, 4, 0, 36, 0 },    /* 朝下 */
    { 0, 18, 40, 0, 40, 36 },   /* 朝左 */
    { 40, 18, 0, 0, 0, 36 },    /* 朝右 */
};
static const color_t k_rot_colors[TRI_ORIENT_N] = {
    C_RED, C_YELLOW, C_LIME, C_MAGENTA,
};

/* 以 (x, y) 为锚点、dir 朝向放置三角形（顶点按绝对坐标交给 set_points，view 自动定位）。 */
static void sc_triangle_place(sc_obj_t *obj, int16_t x, int16_t y, uint8_t dir)
{
    const int16_t *o = k_orients[dir % TRI_ORIENT_N];
    sc_triangle_set_points(obj, (int16_t)(x + o[0]), (int16_t)(y + o[1]),
                           (int16_t)(x + o[2]), (int16_t)(y + o[3]),
                           (int16_t)(x + o[4]), (int16_t)(y + o[5]));
}

static void sc_triangle_demo_rotate_cb(sc_obj_t *obj, const sc_event_t *event)
{
    if (event->type != SC_EVENT_TOUCH_UP)
    {
        return;
    }
    s_rot_idx = (uint8_t)((s_rot_idx + 1U) % TRI_ORIENT_N);
    sc_triangle_place(obj, s_rot_x, s_rot_y, s_rot_idx);
    sc_obj_set_fg(obj, k_rot_colors[s_rot_idx]);
}

void sc_triangle_demo_init(void)
{
    sc_obj_t *screen = sc_page_create(NULL, &k_page, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, 1U);

    sc_obj_t *title = sc_label_create(screen, &k_title);
    sc_obj_set_text(title, "Triangle demo", SC_FONT_DEFAULT);
    sc_obj_set_pos(title, 12, 8);

    /* 4 个静态三角形：上 / 下 / 左 / 右。 */
    sc_obj_t *up = sc_triangle_create(screen, &k_tri_up);
    k_tri_up.color = C_WHITE;
    k_tri_up.fill = C_DODGER_BLUE;
    sc_triangle_place(up, 30, 50, 0);

    sc_obj_t *down = sc_triangle_create(screen, &k_tri_down);
    k_tri_down.color = C_WHITE;
    k_tri_down.fill = C_GREEN;
    sc_triangle_place(down, 110, 50, 1);

    sc_obj_t *left = sc_triangle_create(screen, &k_tri_left);
    k_tri_left.color = C_WHITE;
    k_tri_left.fill = C_ORANGE;
    sc_triangle_place(left, 190, 50, 2);

    sc_obj_t *right = sc_triangle_create(screen, &k_tri_right);
    k_tri_right.color = C_WHITE;
    k_tri_right.fill = C_MAGENTA;
    sc_triangle_place(right, 270, 50, 3);

    /* 交互三角形：点击旋转朝向 + 换色（验证回调 + 三角形内精确命中）。 */
    s_rot_x = 140;
    s_rot_y = 130;
    s_rot_idx = 0U;
    sc_obj_t *rot = sc_triangle_create(screen, &k_tri_rot);
    k_tri_rot.color = C_WHITE;
    k_tri_rot.fill = k_rot_colors[0];
    sc_triangle_place(rot, s_rot_x, s_rot_y, s_rot_idx);
    sc_obj_set_callback(rot, sc_triangle_demo_rotate_cb);

    sc_obj_t *hint = sc_label_create(screen, &k_hint);
    sc_obj_set_text(hint, "up / down / left / right   tap RED to rotate", SC_FONT_DEFAULT);
    sc_obj_set_pos(hint, 12, SC_SCREEN_HEIGHT - 28);

    sc_invalidate_all();
}

#endif /* SC_USER_OBJ && SC_WIDGET_TRIANGLE_ENABLE */
