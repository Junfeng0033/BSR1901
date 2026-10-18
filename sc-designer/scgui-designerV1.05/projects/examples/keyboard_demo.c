#include "keyboard_demo.h"
#include "../src/sc_draw.h"

#if SC_USER_OBJ

#include "../src/sc_mem.h"

#define SC_KEYBOARD_DEMO_TEXT_CAPACITY 64U

typedef struct sc_keyboard_demo_cursor_t
{
    sc_composite_t composite;
    int16_t x;
    int16_t y;
    uint8_t height;
} sc_keyboard_demo_cursor_t;

static sc_label_t k_title_props;
static sc_text_t k_output_props;
static sc_container_t k_page;
static sc_keyboard_t k_keyboard_props;
static sc_keyboard_demo_cursor_t k_cursor_props;
static sc_obj_t *k_output_obj;
static char k_text[SC_KEYBOARD_DEMO_TEXT_CAPACITY];
static uint8_t k_cursor;

const char *sc_keyboard_demo_text(void)
{
    return k_text;
}

uint8_t sc_keyboard_demo_cursor(void)
{
    return k_cursor;
}

static uint8_t sc_keyboard_demo_length(void)
{
    return (uint8_t)sc_strlen(k_text);
}

static void sc_keyboard_demo_locate_cursor(void)
{
    const lv_font_t *font = sc_get_font(k_output_props.font_id);
    const char *cursor = &k_text[k_cursor];
    const char *line_start = k_text;
    int line_height = k_output_props.line_height > 0U
                          ? k_output_props.line_height
                          : ((font && font->line_height > 0) ? font->line_height : 8);
    int16_t base_y = (int16_t)(k_output_obj->view.y + k_output_props.offs_y);
    int16_t cursor_x = k_output_obj->view.x;

    for (;;)
    {
        const char *line_end = NULL;
        const char *next_line;
        uint16_t line_width = 0U;
        int16_t line_x;
        if (*line_start == '\0')
        {
            cursor_x = k_output_obj->view.x;
            break;
        }
        next_line = sc_text_next_line(line_start,
                                      font,
                                      0U,
                                      k_output_obj->view.w,
                                      &line_end,
                                      &line_width);
        if ((k_output_props.align & ALIGN_HOR) == ALIGN_HOR ||
            (k_output_props.align & ALIGN_RIGHT) != 0U)
        {
            line_x = (int16_t)(k_output_obj->view.x +
                               sc_align_offset_x(k_output_obj->view.w,
                                                 line_width,
                                                 k_output_props.align));
        }
        else
        {
            line_x = k_output_obj->view.x;
        }
        if (cursor <= line_end)
        {
            cursor_x = (int16_t)(line_x +
                                 sc_lvgl_measure_text_range(font,
                                                            line_start,
                                                            cursor,
                                                            0U));
            break;
        }
        if (!next_line || next_line <= line_start)
        {
            cursor_x = line_x;
            break;
        }
        line_start = next_line;
        base_y = (int16_t)(base_y + line_height);
    }
    if (cursor_x >= k_output_obj->view.x + (int16_t)k_output_obj->view.w)
    {
        cursor_x = (int16_t)(k_output_obj->view.x + (int16_t)k_output_obj->view.w - 1);
    }
    k_cursor_props.x = cursor_x;
    k_cursor_props.y = base_y;
    k_cursor_props.height = (uint8_t)line_height;
}

static void sc_keyboard_demo_refresh(void)
{
    sc_text_box_sync_text(k_output_obj, k_text);
    sc_keyboard_demo_locate_cursor();
}

static bool sc_keyboard_demo_insert(char ch)
{
    uint8_t len = sc_keyboard_demo_length();
    if (len + 1U >= SC_KEYBOARD_DEMO_TEXT_CAPACITY)
    {
        return false;
    }
    sc_memmove(&k_text[k_cursor + 1U], &k_text[k_cursor], (size_t)(len - k_cursor + 1U));
    k_text[k_cursor++] = ch;
    return true;
}

static void sc_keyboard_demo_key(sc_obj_t *obj, uint8_t key)
{
    uint8_t len = sc_keyboard_demo_length();
    (void)obj;
    if (key >= 0x20U && key < 0x7FU)
    {
        sc_keyboard_demo_insert((char)key);
    }
    else
    {
        switch (key)
        {
        case SC_KEYBOARD_KEY_NEWLINE:
            sc_keyboard_demo_insert('\n');
            break;
        case SC_KEYBOARD_KEY_BACKSPACE:
            if (k_cursor != 0U)
            {
                sc_memmove(&k_text[k_cursor - 1U], &k_text[k_cursor],
                        (size_t)(len - k_cursor + 1U));
                --k_cursor;
            }
            break;
        case SC_KEYBOARD_KEY_DELETE:
            if (k_cursor < len)
            {
                sc_memmove(&k_text[k_cursor], &k_text[k_cursor + 1U],
                        (size_t)(len - k_cursor));
            }
            break;
        case SC_KEYBOARD_KEY_LEFT:
            if (k_cursor != 0U)
            {
                --k_cursor;
            }
            break;
        case SC_KEYBOARD_KEY_RIGHT:
            if (k_cursor < len)
            {
                ++k_cursor;
            }
            break;
        case SC_KEYBOARD_KEY_UP:
            k_cursor = 0U;
            break;
        case SC_KEYBOARD_KEY_DOWN:
            k_cursor = len;
            break;
        default:
            break;
        }
    }
    sc_keyboard_demo_refresh();
}

static void sc_keyboard_demo_cursor_draw(sc_pfb_t *pfb,
                                         const sc_obj_t *obj,
                                         const sc_composite_t *composite)
{
    const sc_keyboard_demo_cursor_t *cursor =
        (const sc_keyboard_demo_cursor_t *)composite->user;
    int16_t y = cursor->y;
    int16_t bottom = (int16_t)(cursor->y + cursor->height);
    if (y < obj->view.y)
    {
        y = obj->view.y;
    }
    if (bottom > obj->view.y + (int16_t)obj->view.h)
    {
        bottom = (int16_t)(obj->view.y + (int16_t)obj->view.h);
    }
    if (cursor->x >= obj->view.x && cursor->x < obj->view.x + (int16_t)obj->view.w &&
        bottom > y)
    {
        sc_draw_fill_rect(pfb, cursor->x, y, 1, bottom - y, C_BLACK);
    }
}

static const sc_composite_hooks_t k_cursor_hooks = {
    .draw = sc_keyboard_demo_cursor_draw,
};

void sc_keyboard_demo_init(void)
{
    sc_obj_t *screen = sc_page_create(NULL, &k_page, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, 1U);
    sc_obj_t *title;
    sc_obj_t *keyboard;
    sc_obj_t *cursor;

    k_text[0] = '\0';
    k_cursor = 0U;

    title = sc_label_create(screen, &k_title_props);
    sc_obj_set_text(title, "Composite Keyboard Demo", SC_FONT_DEFAULT);
    sc_obj_set_pos(title, 8, 4);

    k_output_obj = sc_text_box_create(screen, &k_output_props);
    sc_obj_set_rect(k_output_obj, 8, 24, 304, 60, ALIGN_NONE);
    sc_obj_set_fg(k_output_obj, C_BLACK);
    sc_obj_set_bg(k_output_obj, C_WHITE);

    keyboard = sc_keyboard_create(screen,
                                  &k_keyboard_props,
                                  SC_KEYBOARD_MODE_ASCII,
                                  sc_keyboard_demo_key);
    sc_obj_set_rect(keyboard, 0, 92, SC_SCREEN_WIDTH,
                    (uint16_t)(SC_SCREEN_HEIGHT - 92), ALIGN_NONE);
    sc_obj_set_key_focus(keyboard);

    cursor = sc_composite_create(screen,
                                 &k_cursor_props.composite,
                                 &k_cursor_hooks,
                                 &k_cursor_props,
                                 0U);
    sc_obj_set_rect(cursor,
                    k_output_obj->view.x,
                    k_output_obj->view.y,
                    k_output_obj->view.w,
                    k_output_obj->view.h,
                    ALIGN_NONE);
    cursor->flags &= (uint8_t)~SC_OBJ_F_FOCUSABLE;

    sc_keyboard_demo_refresh();
    sc_invalidate_all();
}

#endif
