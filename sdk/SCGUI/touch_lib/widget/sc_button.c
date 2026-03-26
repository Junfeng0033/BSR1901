

#include "sc_obj.h"

static bool handle_event_Button(sc_obj_t *obj, sc_event_t *event)
{
    Button_t *p = (Button_t *)obj;
    if (event->type == SC_EVENT_TYPE_DRAW)
    {
        sc_pfb_t *dest=event->dat.arg;
        dest->parent = &obj->rect;
        sc_draw_Rounded_rect(dest, obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h, p->r, p->ir, p->border_color, p->color, obj->alpha);
        sc_draw_Label(dest, &p->Label, p->border_color,p->color, obj->alpha);
        dest->parent = NULL;
        return true;
    }
    return false;
}

sc_obj_t *sc_create_Button(sc_obj_t *parent, int x, int y, int w, int h, sc_align_t align)
{
    Button_t *p = (Button_t *)sc_obj_init(parent, malloc(sizeof(Button_t)), SC_OBJ_TYPE_BUTTON); // 初始化基类
    if (p)
    {
        p->base.handle_event = handle_event_Button;                                           // 设置事件处理函数
        sc_obj_set_geometry(&p->base, x, y, w, h, align);                      // 初始化基类位置大小
        sc_init_Label(&p->base.rect, 0, 0, &p->Label, gui->font, NULL, ALIGN_CENTER); // 初始化Label
        p->color = gui->fc;
        p->border_color = gui->bkc;
        p->r = 0;
        p->ir = 0;
        return &p->base;
    }
    return NULL;
}

void sc_set_button_radius(sc_obj_t *obj, uint8_t r,uint8_t ir,color_t color, color_t border_color)
{
    if(obj==NULL) return;
    Button_t *p = (Button_t *)obj;
    p->r = r;
    p->ir = ir;
    p->color = color;
    p->border_color = border_color;
    sc_obj_dirty(obj, NULL);
}

void sc_set_button_text(sc_obj_t *obj, char *text)
{
    if(obj==NULL) return;
    Button_t *p = (Button_t *)obj;
    p->Label.text = text;
    sc_obj_dirty(obj, NULL);
}
