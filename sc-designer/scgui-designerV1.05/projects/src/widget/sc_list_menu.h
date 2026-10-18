#ifndef SC_WIDGET_LIST_MENU_H
#define SC_WIDGET_LIST_MENU_H

#include "../sc_common.h"
#include "../sc_obj.h"
#include "sc_image.h"

/* 多级菜单条目：图示 + 文本。
 * 文本以 Back/Break/Exit 结尾 = 返回项（ENTER 等价 CMD_BACK）。 */
typedef struct sc_menu_item_t
{
    const sc_image_t *icon; /* 图示在前；NULL = 仅文本，图示列仍保留对齐 */
    const char *text;
} sc_menu_item_t;

#define SC_MENU_ITEM(text_, icon_) \
    {(icon_), (text_)}

/* 菜单层级深：顶层 0 级，最多下钻 SC_MENU_DEPTH_MAX-1 级 */
#define SC_MENU_DEPTH_MAX 3U

/* Slide-back gesture threshold (px): left swipe beyond this triggers back. */
#define SC_MENU_SLIDE_BACK_PX 40

/* 菜单节点 = 一级条目集合。parent_item 指向父级中打开本级的那个条目，
 * ENTER 时 find_child 按 parent_item 匹配进子级，BACK 时 find_parent 回退。 */
typedef struct sc_menu_node_t
{
    const sc_menu_item_t *items;
    const sc_menu_item_t *parent_item; /* 顶层节点填 NULL */
    void (*items_cb)(void *arg);       /* 本级叶子 ENTER 回调，arg=菜单控件对象（NULL 无动作） */
    uint16_t items_cnt;
} sc_menu_node_t;

#define SC_MENU_NODE(items_, parent_item_, cb_, cnt_) \
    {(items_), (parent_item_), (cb_), (cnt_)}

typedef struct sc_list_menu_t
{
    /* 4 字节指针组 */
    const sc_menu_node_t *nodes; /* 扁平节点表（含所有级） */
    const sc_menu_node_t *head;  /* 当前级节点 */
    /* 2 字节组 */
    int16_t yofs;                /* 当前级滚动偏移 */
    color_t fg;                  /* 文本色（兼圆角边框色、分界水平线色） */
    color_t bg;                  /* 面板底色 */
    color_t sel_bg;              /* 选中条目底色 */
    /* 1 字节组（sizeof=28，4 对齐零填充） */
    uint8_t cursor[SC_MENU_DEPTH_MAX]; /* 各级光标 */
    uint8_t nodes_cnt;           /* 节点表数量（级数 ≤ 255） */
    int8_t icon_x;               /* 图示水平位置（相对行左，用户自调） */
    int8_t line_xs;              /* 分界线/选中色块起点（相对行左），字符再 +2px */
    uint8_t level;               /* 当前级数：0=顶层 */
    uint8_t item_h;              /* 行内容高（图示/文字垂直居中，分界线贴行底），0=自动 */
    uint8_t item_gap;            /* 行间留白（纯空隙，无线） */
    uint8_t font_id;
    uint8_t radius;              /* 圆角 */
    uint8_t bar_w;               /* 滚动条宽，0 隐藏 */
    uint8_t cb_view;   /* 1: callback view active; 0: normal list */
} sc_list_menu_t;

void sc_lcd_draw_list_menu(sc_pfb_t *pfb, int16_t x, int16_t y, uint16_t w, uint16_t h,
                           const sc_list_menu_t *props);

#if SC_USER_OBJ && SC_WIDGET_LIST_MENU_ENABLE

sc_obj_t *sc_list_menu_create(sc_obj_t *parent, sc_list_menu_t *props);
void sc_list_menu_set_data(sc_obj_t *obj, const sc_menu_node_t *nodes, uint16_t nodes_cnt);
const sc_menu_item_t *sc_list_menu_current(sc_obj_t *obj);
bool sc_list_menu_style(sc_obj_t *obj, sc_style_t style, const void *value);
bool sc_obj_handle_list_menu_event(sc_obj_t *obj, const sc_event_t *event);

#endif /* SC_USER_OBJ && SC_WIDGET_LIST_MENU_ENABLE */

#endif
