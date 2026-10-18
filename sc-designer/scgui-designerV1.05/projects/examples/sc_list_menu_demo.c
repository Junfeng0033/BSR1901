#include "sc_list_menu_demo.h"

#if SC_USER_OBJ && SC_WIDGET_LIST_MENU_ENABLE

/* 8x8 RGB565 占位图示（边框+填充双色块）。真实工程替换为图标数组即可。 */
#define IC_B C_RGB(220, 220, 220)
#define IC_F C_RGB(60, 120, 200)
#define IC_R8(b_, f_)                   \
    b_, b_, b_, b_, b_, b_, b_, b_,     \
        b_, f_, f_, f_, f_, f_, f_, b_, \
        b_, f_, f_, f_, f_, f_, f_, b_, \
        b_, f_, f_, f_, f_, f_, f_, b_, \
        b_, f_, f_, f_, f_, f_, f_, b_, \
        b_, f_, f_, f_, f_, f_, f_, b_, \
        b_, f_, f_, f_, f_, f_, f_, b_, \
        b_, b_, b_, b_, b_, b_, b_, b_

static const uint16_t k_icon_set[64] = {IC_R8(IC_B, IC_F)};
static const uint16_t k_icon_disp[64] = {IC_R8(IC_F, IC_B)};
static const uint16_t k_icon_sub[64] = {IC_R8(IC_B, C_RGB(200, 140, 40))};

static const sc_image_t k_img_set = SC_IMAGE_NEW(k_icon_set, 8, 8, SC_IMAGE_RGB565);
static const sc_image_t k_img_disp = SC_IMAGE_NEW(k_icon_disp, 8, 8, SC_IMAGE_RGB565);
static const sc_image_t k_img_sub = SC_IMAGE_NEW(k_icon_sub, 8, 8, SC_IMAGE_RGB565);

/* 菜单显示区域（公共回调里的居中 label 复用同一尺寸） */
#define DEMO_MENU_X 12
#define DEMO_MENU_Y 56
#define DEMO_MENU_W 256
#define DEMO_MENU_H 124

/* 条目表：文本以 Back 结尾 = 返回项 */
static void menu_leaf_cb(void *arg); /* 前向声明：节点表引用 */

static const sc_menu_item_t k_net_items[] = {
    SC_MENU_ITEM("WiFi", &k_img_sub),
    SC_MENU_ITEM("BLE", &k_img_sub),
    SC_MENU_ITEM("Back", NULL),
};

static const sc_menu_item_t k_set_items[] = {
    SC_MENU_ITEM("Brightness", &k_img_set),
    SC_MENU_ITEM("Volume", &k_img_set),
    SC_MENU_ITEM("Network", NULL),
    SC_MENU_ITEM("Back", NULL),
};

static const sc_menu_item_t k_disp_items[] = {
    SC_MENU_ITEM("Theme", &k_img_disp),
    SC_MENU_ITEM("Font", &k_img_disp),
    SC_MENU_ITEM("Back", NULL),
};

static const sc_menu_item_t k_main_items[] = {
    SC_MENU_ITEM("Settings", NULL),
    SC_MENU_ITEM("Display", NULL),
    SC_MENU_ITEM("About", &k_img_set),
};

/* 扁平节点表：parent_item 指向父级中打开本级的条目（顺序不限，顶层在前即可） */
static const sc_menu_node_t k_nodes[] = {
    SC_MENU_NODE(k_main_items, NULL, menu_leaf_cb, SC_ARRAY_SIZE(k_main_items)),
    SC_MENU_NODE(k_set_items, &k_main_items[0], menu_leaf_cb, SC_ARRAY_SIZE(k_set_items)),
    SC_MENU_NODE(k_disp_items, &k_main_items[1], menu_leaf_cb, SC_ARRAY_SIZE(k_disp_items)),
    SC_MENU_NODE(k_net_items, &k_set_items[2], menu_leaf_cb, SC_ARRAY_SIZE(k_net_items)),
};

static sc_container_t k_page;
static sc_container_t k_panel;
static sc_label_t k_title;
static sc_label_t k_hint;
static sc_list_menu_t k_menu_props;
static sc_obj_t *k_menu_obj;

/* Shared leaf callback: centered label with the selected item + a parameter
 * progress bar (touch-drag or arrow keys in edit mode, e.g. volume).
 * BACK/left-slide returns; wire business logic here. */
static void menu_leaf_cb(void *arg)
{
    static sc_label_t k_leaf_label;                                               /* callback-view label: shows selected item */
    static sc_progress_t k_leaf_progress = SC_PROGRESS_NEW(C_GREEN, C_GRAY, 50U); /* parameter bar (e.g. volume) */
    sc_obj_t *menu = (sc_obj_t *)arg;
    const sc_menu_item_t *item = sc_list_menu_current(menu);
    sc_obj_t *label = sc_label_create(menu, &k_leaf_label);
    sc_obj_set_rect(label, 0, -20, 100, 40, ALIGN_CENTER);
    sc_obj_set_fg(label, C_GREEN);
    sc_obj_set_text(label, item ? item->text : "label", SC_FONT_DEFAULT);
    /* parameter bar: touch-drag or arrow keys in edit mode (e.g. volume) */
    sc_obj_t *pg = sc_progress_create(menu, &k_leaf_progress);
    sc_obj_set_rect(pg, 0, 0, 120, 16, ALIGN_CENTER);
}

void sc_list_menu_demo_init(void)
{
    sc_obj_t *screen = sc_page_create(NULL, &k_page, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, 1U);

    sc_obj_t *panel = sc_container_create(screen, &k_panel);
    sc_container_set_flags(panel, SC_CONTAINER_F_BORDER);
    sc_obj_set_bg(panel, C_RGB(24, 32, 44));
    sc_obj_set_border(panel, C_CYAN);
    sc_obj_set_rect(panel, 20, 16, 280, 208, ALIGN_NONE);

    sc_obj_t *title = sc_label_create(panel, &k_title);
    sc_obj_set_fg(title, C_YELLOW);
    sc_obj_set_text(title, "sc_list_menu demo", SC_FONT_DEFAULT);
    sc_obj_set_pos(title, 12, 10);

    sc_obj_t *hint = sc_label_create(panel, &k_hint);
    sc_obj_set_fg(hint, C_WHITE);
    sc_obj_set_text(hint, "UP/DOWN move, ENTER in, BACK out", SC_FONT_DEFAULT);
    sc_obj_set_pos(hint, 12, 30);

    k_menu_obj = sc_list_menu_create(panel, &k_menu_props);
    sc_obj_set_rect(k_menu_obj, DEMO_MENU_X, DEMO_MENU_Y, DEMO_MENU_W, DEMO_MENU_H, ALIGN_NONE);
    k_menu_props.icon_x = 8;   /* 图示水平位置（相对菜单行左，自调） */
    k_menu_props.line_xs = 32; /* 分界线/选中色块起点（相对菜单行左，自调），字符再 +2px */
    k_menu_props.item_h = 24;  /* 行内容高：图示/文字垂直居中，分界线贴行底 */
    k_menu_props.item_gap = 8; /* 行间留白 */
    sc_obj_set_key_focus(k_menu_obj);
    sc_list_menu_set_data(k_menu_obj, k_nodes, (uint16_t)SC_ARRAY_SIZE(k_nodes));

    sc_invalidate_all();
}

#endif
