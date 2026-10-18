#include "image_demo.h"

#include "watch.h"

#include "../src/sc_event.h"
#include "../src/widget/sc_label.h"

#if SC_USER_OBJ && SC_WIDGET_IMAGE_ENABLE

static sc_container_t k_page;
static sc_obj_t *k_watch_image;
static sc_image_props_t k_watch = { &watch, NULL }; /* image 常量资源 + 控件回调 */
static sc_obj_t *k_counter;
static sc_label_t k_counter_label;

/* 测试：公共事件 + 对象级回调。按下显示 PRESSED，释放显示 released。 */
static void sc_image_demo_cb(sc_obj_t *obj, const sc_event_t *event)
{
    (void)obj;
    if (event->type == SC_EVENT_KEY_DOWN || event->type == SC_EVENT_TOUCH_DOWN)
    {
        sc_obj_set_text(k_counter, "PRESSED", SC_FONT_DEFAULT);
    }
    else if (event->type == SC_EVENT_KEY_UP || event->type == SC_EVENT_TOUCH_UP)
    {
        sc_obj_set_text(k_counter, "released", SC_FONT_DEFAULT);
    }
}

void sc_image_demo_init(void)
{
    sc_obj_t *screen = sc_page_create(NULL, &k_page, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, 1U);

#if !PY32_MCU
#include "sc_sdl2.h"
    (void)sc_spiflash_mount_file(WATCH_QOI_BLOB_ADDR, "watch.bin");
#endif

    k_watch_image = sc_img_create(screen, &k_watch);
    sc_obj_set_rect(k_watch_image,
                    (SC_SCREEN_WIDTH - WATCH_WIDTH) / 2,
                    (SC_SCREEN_HEIGHT - WATCH_HEIGHT) / 2,
                    WATCH_WIDTH, WATCH_HEIGHT, ALIGN_NONE);
    sc_obj_set_callback(k_watch_image, sc_image_demo_cb); /* 注册回调自动开启 FOCUSABLE */

    k_counter = sc_label_create(screen, &k_counter_label);
    sc_obj_set_text(k_counter, "touch image", SC_FONT_DEFAULT);
    sc_obj_set_rect(k_counter, 0, SC_SCREEN_HEIGHT - 24, 120U, 20U, ALIGN_HOR);

    sc_invalidate_all();
}

#elif SC_USER_OBJ

void sc_image_demo_init(void)
{
}

#endif
