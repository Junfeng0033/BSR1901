#include "gif_demo.h"

#include "gif_qoia.h"

#include "../src/sc_event.h"
#if !PY32_MCU
#include "sc_sdl2.h"
#endif

#if SC_USER_OBJ && SC_WIDGET_IMAGE_ENABLE

#define GIF_DEMO_MS 50U

static sc_container_t k_page;
static sc_obj_t *k_gif_image;
static uint8_t k_gif_frame;

#if SC_WIDGET_LABEL_ENABLE
static sc_label_t k_lbl_flash;
static sc_label_t k_lbl_lossy;
static sc_label_t k_lbl_hint;
#endif

static void sc_gif_demo_timer(sc_delay_t *timer)
{
    sc_obj_t *img = (sc_obj_t *)timer->arg;
    if (!img) return;
    k_gif_frame = (uint8_t)((k_gif_frame + 1U) % GIF_QOIA_FRAME_COUNT);
    sc_image_set_img(img, &gif_qoia[k_gif_frame]);
    sc_reload_delay(timer, GIF_DEMO_MS);
}

void sc_gif_demo_init(void)
{
    sc_obj_t *screen = sc_page_create(NULL, &k_page, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, 1U);

#if !PY32_MCU
    (void)sc_spiflash_mount_file(GIF_QOIA_FLASH_BASE, "gif_qoia.bin");
#endif

#if SC_WIDGET_LABEL_ENABLE
    /* Under GIF: draw first so flash QOI A4 blends on top. */
    {
        sc_obj_t *lbl = sc_label_create(screen, &k_lbl_flash);
        sc_obj_set_text(lbl, "flash: gif_qoia.bin", SC_FONT_DEFAULT);
        sc_obj_set_pos(lbl, 12, 48);

        lbl = sc_label_create(screen, &k_lbl_lossy);
        sc_obj_set_text(lbl, "indexQOI lossy + A4", SC_FONT_DEFAULT);
        sc_obj_set_pos(lbl, 12, 88);

        lbl = sc_label_create(screen, &k_lbl_hint);
        sc_obj_set_text(lbl, "OK: no occlusion", SC_FONT_DEFAULT);
        sc_obj_set_pos(lbl, 12, 128);
    }
#endif

    k_gif_frame = 0U;
    static sc_image_props_t k_gif_img = { &gif_qoia[0], NULL };
    k_gif_image = sc_img_create(screen, &k_gif_img);
    sc_obj_set_rect(k_gif_image, 0, 0, GIF_QOIA_WIDTH, GIF_QOIA_HEIGHT, ALIGN_NONE);

    sc_add_delay(sc_gif_demo_timer, GIF_DEMO_MS, k_gif_image);
    sc_invalidate_all();
}

#elif SC_USER_OBJ

void sc_gif_demo_init(void)
{
}

#endif
