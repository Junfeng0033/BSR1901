
#include "sc_obj.h"

static int16_t g_pos_mark = 0;      // 光标位置标记
static sc_rect_t g_pos = {0};       // 光标位置
static Keyboard_t g_keyboard = {0}; // 静态内存，键盘只有一个

// 创建键盘对象,键盘对象为静态只有一个
static sc_obj_t *sc_create_keyboard(sc_obj_t *parent, int x, int y, int w, int h, sc_align_t align);

// 获取输入框光标位置
static bool sc_get_line_edit_pos(sc_label_t *label, sc_rect_t *pos, int16_t x, int16_t y, int cmd)
{
    if (label == NULL || label->font == NULL || label->text == NULL)
        return false;
    sc_rect_t *parent = label->box;
    lv_font_t *font = label->font;
    lv_font_glyph_dsc_t g;
    unicode_t unicode[SC_UINCODE_SIZE];
    uint16_t total_height = 0;
    uint16_t glyph_cnt = 0;
    uint32_t indx = 0;
    uint16_t line_width_max = parent != NULL ? parent->w : gui->lcd_rect.w;
    if (cmd)
    {
        x = pos->x;
        y = pos->y;
        if (cmd == KB_VALUE_RIGHT)
        {
            g_pos_mark++;
        }
        else if (cmd == KB_VALUE_LEFT)
        {
            if (g_pos_mark <= 0)
                return false;
            g_pos_mark--;
        }
        else if (cmd == KB_VALUE_UP)
        {
            y = y - pos->h;
            cmd = 0; // 模拟触控
        }
        else if (cmd == KB_VALUE_DOWN)
        {
            y = y + pos->h;
            cmd = 0; // 模拟触控
        }
    }
    do
    {
        font_dsc_t fdsc = sc_get_line_width(line_width_max - label->xoffs, label, &indx, unicode, &g);
        int xs = label->xoffs;
        int ys = label->yoffs;
        sc_parent_align(parent, &xs, &ys, fdsc.line_width, fdsc.line_height, label->align);
        sc_rect_t box = {.x = label->box->x, .y = ys + total_height, .w = label->box->w, .h = fdsc.line_height};
        if (cmd != 0)
        {
            box.x = xs;
            for (int k = 0; k < fdsc.line_cnt; k++, glyph_cnt++) // 通过g_pos_mark查找光标
            {
                font->get_glyph_dsc(font, &g, unicode[k], 0);
                box.w = g.adv_w + fdsc.Xspace;
                if (glyph_cnt >= g_pos_mark)
                {
                    *pos = box; // 找到位置返回
                    return true;
                }
                box.x += box.w;
            }
            if (label->text[indx - 1] == 0)
            {
                *pos = box; // 找不到返回最后一个
                if (g_pos_mark >= glyph_cnt)
                {
                    g_pos_mark = glyph_cnt - 1;
                    return false;
                }
                return true;
            }
        }
        else if (sc_rect_touch_ctx(&box, x, y)) // 通过位置查找光标
        {
            box.x = xs;
            for (int k = 0; k < fdsc.line_cnt; k++)
            {
                if (font->get_glyph_dsc(font, &g, unicode[k], 0) == 0)
                    continue;
                box.w = g.adv_w + fdsc.Xspace;
                if (x < xs)
                {
                    break; // 如果点在字符外左边返回第一个
                }
                if (sc_rect_touch_ctx(&box, x, y)) // 字符内
                {
                    int right_half = box.x + box.w / 2;                                   // 字符中心
                    if ((x > right_half) && font->get_glyph_dsc(font, &g, unicode[k], 0)) // 下一个
                    {
                        box.w = g.adv_w + fdsc.Xspace;
                        box.x += box.w;
                        glyph_cnt++;
                    }
                    break;
                }
                box.x += box.w;
                glyph_cnt++;
            }
            *pos = box;             // 记录位置
            g_pos_mark = glyph_cnt; // 记录mark下标
            return true;
        }
        else
        {
            glyph_cnt += fdsc.line_cnt;
        }
        total_height += fdsc.line_height;
    } while (label->text[indx - 1] != 0); // 判断是否结束符
    return false;
}

// 绘制编辑光标
static void sc_draw_line_edit_pos(Line_edit_t *edit)
{
    edit->pos_x = g_pos.x - edit->base.rect.x;
    edit->pos_y = g_pos.y - edit->base.rect.y;
    sc_dirty_mark(NULL, &g_pos);
}

// 清除编辑光标
static void sc_clear_line_edit_pos(Line_edit_t *edit)
{
    sc_rect_t last_pos = {edit->base.rect.x + edit->pos_x, edit->base.rect.y + edit->pos_y, g_pos.w, g_pos.h};
    sc_dirty_mark(NULL, &last_pos);
}

// 键盘输入字符
static bool sc_keyboard_line_edit(Line_edit_t *edit, int cmd)
{
    if (edit == NULL || edit->editbuf_size == 0)
        return false;
    char *p = &edit->Label.text[g_pos_mark];
    switch (cmd)
    {
    case KB_VALUE_UP:
    case KB_VALUE_DOWN:
    case KB_VALUE_LEFT:
    case KB_VALUE_RIGHT:
        if (sc_get_line_edit_pos(&edit->Label, &g_pos, 0, 0, cmd))
        {
            sc_clear_line_edit_pos(edit);
            sc_draw_line_edit_pos(edit);
            return true;
        }
        break;
    case KB_VALUE_BACKSPACE:
        if (g_pos_mark > 0)
        {
            p--;
            do
            {
                p[0] = p[1];
                p++;
            } while (*p);
            g_pos_mark--;                                          // 光标前移
            sc_get_line_edit_pos(&edit->Label, &g_pos, 0, 0, cmd); // 右
            sc_draw_line_edit_pos(edit);
            sc_obj_dirty((sc_obj_t *)edit, NULL);
            return true;
        }
        break;
    case KB_VALUE_DEL:
        if (p[0])
        {
            while (*p)
            {
                p[0] = p[1];
                p++;
            }
            sc_obj_dirty((sc_obj_t *)edit, NULL);
            return true;
        }
        break;
    default:
        if (((cmd <= '~') && (cmd >= ' ')) || cmd == KB_VALUE_NEWLINE)
        {
            char *text_buf = edit->Label.text;
            uint16_t curr_len = strlen(text_buf); // 英文下字符数=字节数
            if (curr_len < edit->editbuf_size)
            {
                // 光标在末尾：直接追加到最后（光标后无字符）
                if (g_pos_mark >= curr_len)
                {
                    text_buf[curr_len] = cmd;
                    text_buf[curr_len + 1] = '\0';
                }
                else
                {
                    for (int i = curr_len; i > g_pos_mark; i--)
                    {
                        text_buf[i] = text_buf[i - 1];
                    }
                    text_buf[g_pos_mark] = cmd;
                }
                g_pos_mark++;                                          // 光标后移（跟随插入的字符）
                sc_get_line_edit_pos(&edit->Label, &g_pos, 0, 0, cmd); // 右
                sc_draw_line_edit_pos(edit);
                sc_obj_dirty((sc_obj_t *)edit, NULL);
                return true;
            }
        }
        break;
    }
    return false;
}

// 编辑框事件
static bool handle_event_line_edit(sc_obj_t *obj, sc_event_t *event)
{
    Line_edit_t *edit = (Line_edit_t *)obj;
    if (event->type == SC_EVENT_TYPE_DRAW)
    {
        sc_pfb_t *dest=event->dat.arg;
        dest->parent = &obj->rect;                                                                           // 绘制背景
        sc_draw_Frame(dest, obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h, 1, edit->color, obj->alpha); // 绘制矩形
        sc_draw_Label(dest, &edit->Label, edit->color, edit->border_color, obj->alpha);                      // 绘制文本
        if (obj->flag & SC_OBJ_FLAG_FOCUS)                                                                   // 绘制光标
        {
            sc_draw_Fill(dest, obj->rect.x + edit->pos_x, obj->rect.y + edit->pos_y, 1, g_pos.h, C_RED, 255); // 绘制光标
        }
        dest->parent = NULL;
        return true;
    }
    if (event->type == SC_EVENT_TOUCH_DOWN)
    {
        return true; // 按下聚焦
    }
    if (event->type == SC_EVENT_TYPE_FOCUS)
    {
        obj->flag |= SC_OBJ_FLAG_FOCUS; // 设置焦点

        if (g_keyboard.edit && g_keyboard.edit != edit)
        {
            g_keyboard.edit->base.flag &= ~SC_OBJ_FLAG_FOCUS; // 清除焦点
            sc_clear_line_edit_pos(g_keyboard.edit);          // 清除光标
        }
        if (sc_get_line_edit_pos(&edit->Label, &g_pos, event->dat.pos[0], event->dat.pos[1], 0))
        {
            sc_clear_line_edit_pos(edit); // 清除光标
            sc_draw_line_edit_pos(edit);  // 绘制光标
            return true;
        }
    }
    else if (event->type == SC_EVENT_TOUCH_UP)
    {
        if (g_keyboard.edit == NULL)
        {
            sc_obj_move_srceen(g_touch.srceen, 0, -50); // 上移屏幕，创建键盘
            sc_create_keyboard(g_touch.srceen, 0, 0, LD_CFG_SCREEN_WIDTH, LD_CFG_SCREEN_HEIGHT >> 1, ALIGN_BOTTOM);
        }
        g_keyboard.edit = edit; // 绑定编辑框
        return true;
    }
    else if (event->type == SC_EVENT_TYPE_CMD && g_keyboard.edit)
    {
        // sdl2键盘事件要转换成sc键盘事件
        // sc_keyboard_line_edit(Line_edit_t *edit, int cmd);
    }
    return false;
}

// 创建编辑框
sc_obj_t *sc_create_line_edit(sc_obj_t *parent, int x, int y, int w, int h, sc_align_t align)
{
    Line_edit_t *p = (Line_edit_t *)sc_obj_init(parent, malloc(sizeof(Line_edit_t)), SC_OBJ_TYPE_LINE_EDIT); // 初始化基类
    if (p)
    {
        p->base.handle_event = handle_event_line_edit;    // 设置事件处理函数
        sc_obj_set_geometry(&p->base, x, y, w, h, align); // 初始化基类位置大小
        p->color = gui->fc;
        p->border_color = gui->bkc;
        p->editbuf_size = 0;
        p->pos_x = 0;
        p->pos_y = 0;
        sc_init_Label(&p->base.rect, 2, 0, &p->Label, gui->font, NULL, 0); // 初始化Label
        return &p->base;
    }
    return NULL;
}
// 编辑框绑定文本缓存
void sc_set_edit_text(sc_obj_t *obj, char *text, uint8_t editbuf_size)
{
    if (obj == NULL)
        return;
    Line_edit_t *p = (Line_edit_t *)obj;
    p->Label.text = text;
    p->editbuf_size = editbuf_size;
}

// 键盘事件
static bool handle_event_keyboard(sc_obj_t *obj, sc_event_t *event)
{
    Keyboard_t *p = (Keyboard_t *)obj;
    if (event->type == SC_EVENT_TYPE_DRAW)
    {
        sc_pfb_t *dest=event->dat.arg;
        sc_draw_Rounded_rect(dest, obj->rect.x, obj->rect.y, obj->rect.w, obj->rect.h, 5, 5, gui->fc, p->color, 255);
        // 3. 遍历当前键盘的坐标+文本数组，绘制所有按键
        int xs = obj->rect.x;
        int ys = obj->rect.y;
        for (int y = 0; y < p->kb.row_num; y++)
        {
            for (int x = 0; x < p->kb.col_num; x++)
            {
                const kbBtnInfo_t *kb = &p->kb.kb_tab[y * p->kb.col_num + x];
                color_t colour = (kb == p->kb.now) ? C_RED : p->border_color;
                sc_draw_kb_btn(dest, xs, ys, kb, p->color, colour, p->kb.upper);
            }
        }
        return true;
    }
    if (event->type == SC_EVENT_TOUCH_DOWN)
    {
        return true;
    }
    if (event->type == SC_EVENT_TYPE_FOCUS)
    {
        for (int y = 0; y < p->kb.row_num; y++)
        {
            for (int x = 0; x < p->kb.col_num; x++)
            {
                const kbBtnInfo_t *kb = &p->kb.kb_tab[y * p->kb.col_num + x];
                sc_rect_t rect = sc_get_kb_pos(kb, obj->rect.x, obj->rect.y);
                if (sc_rect_touch_ctx(&rect, event->dat.pos[0], event->dat.pos[1]))
                {
                    sc_dirty_mark(NULL, &rect); //  标记按键区域
                    p->kb.now = kb;
                    int cmd = sc_get_kb_vol(kb, p->kb.upper); // 获取按键值
                    sc_keyboard_line_edit(p->edit, cmd);
                    return true;
                }
            }
        }
    }
    else if (event->type == SC_EVENT_TOUCH_UP)
    {
        if (p->kb.now)
        {
            if (p->kb.now->value == KB_VALUE_ENTER)
            {
                sc_obj_hidden(obj);                       // 隐藏键盘
                sc_clear_line_edit_pos(p->edit);          // 清除光标
                p->edit->base.flag &= ~SC_OBJ_FLAG_FOCUS; // 清除焦点
                p->edit = NULL;
                sc_obj_move_srceen(g_touch.srceen, 0, 50); // 恢复屏幕
            }
            else if (p->kb.now->value >= KB_VALUE_NUMBER_MODE && p->kb.now->value <= KB_VALUE_SHIFT_MODE)
            {
                sc_init_keyboard(&p->kb, p->kb.now->value & 0x0f); // 切换键盘模式
                sc_obj_dirty(obj, NULL);
            }
            else
            {
                sc_rect_t rect = sc_get_kb_pos(p->kb.now, obj->rect.x, obj->rect.y);
                sc_dirty_mark(NULL, &rect);
            }
            p->kb.now = NULL;
        }
    }
    return false;
}
// 创建键盘
static sc_obj_t *sc_create_keyboard(sc_obj_t *parent, int x, int y, int w, int h, sc_align_t align)
{
    Keyboard_t *p = (Keyboard_t *)sc_obj_init(parent, &g_keyboard.base, SC_OBJ_TYPE_KEYBOARD); // 初始化基类
    if (p)
    {
        p->base.handle_event = handle_event_keyboard;     // 设置事件处理函数
        sc_obj_set_geometry(&p->base, x, y, w, h, align); // 初始化基类位置大小
        p->color = C_WHITE;
        p->border_color = C_RGB(100, 100, 100);
        sc_init_keyboard(&p->kb, 3);
        return &p->base;
    }
    return NULL;
}