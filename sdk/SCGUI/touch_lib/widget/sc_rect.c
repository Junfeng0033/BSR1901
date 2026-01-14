#include "sc_obj.h"

static void rect_handle_draw(sc_obj_t *obj,  sc_pfb_t *dest)
{
    // Canvas不做任何绘制
    Rect_t *rect = (Rect_t *)obj;
    dest->parent = &obj->parent->rect;
    sc_draw_Fill(dest, obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h, rect->color,obj->alpha); // 绘制矩形
    dest->parent =NULL;
}

/* 创建矩形：从静态池分配，初始化默认样式 */
sc_obj_t *sc_create_rect(sc_obj_t *parent, int x, int y, int w, int h, sc_align_t align)
{
    sc_obj_t *obj = sc_obj_init(parent, malloc(sizeof(Rect_t)), SC_OBJ_TYPE_RECT); // 初始化基类
    if (obj)
    {
        Rect_t *rect = (Rect_t *)obj;
        rect->base.handle_event = NULL;              // 设置事件处理函数
        rect->base.handle_draw = rect_handle_draw;   // 设置绘制函数
        rect->base.attr = 0;                         // 设置属性
        rect->color = gui->fc;                       // 设置属性
        rect->border_color = gui->bc;                // 设置属性
        sc_obj_set_geometry(obj, x, y, w, h, align); // 初始化基类位置大小
    }
    return obj;
}

sc_obj_t *sc_create_srceen(sc_obj_t *parent, int x, int y, int w, int h, sc_align_t align)
{
    sc_obj_t *obj = sc_obj_init(parent, malloc(sizeof(sc_obj_t)), SC_OBJ_TYPE_SCREEN); // 初始化基类
    if (obj)
    {
        obj->handle_event = NULL;        // 设置事件处理函数
        obj->handle_draw = NULL;         // 设置绘制函数
        obj->attr |= SC_OBJ_ATTR_VIRTUAL; // 设置属性
        sc_obj_set_geometry(obj, x, y, w, h, align); // 初始化基类位置大小
    }
    return obj;
}