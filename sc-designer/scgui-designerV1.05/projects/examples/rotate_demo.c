#include "rotate_demo.h"
#include "sc_demo_assets.h"

#if SC_USER_OBJ && SC_WIDGET_TRANSFORM_ENABLE

#define ROTATE_DEMO_MS 40U
#define ROTATE_DEMO_STEP 6
#define ROTATE_SCALE_BASE 256 /* 1.0 in Q8 */
#define ROTATE_SCALE_AMP 128  /* ±0.5 */

static sc_container_t k_page;
static Transform_t k_tf;
static sc_obj_t *k_rotate_obj;
static int k_angle;
static const sc_image_t k_sun_image =
    SC_IMAGE_NEW(img_sun, IMG_SUN_WIDTH, IMG_SUN_HEIGHT, SC_IMAGE_RGB565);

static void sc_rotate_demo_timer(sc_delay_t *timer)
{
    if (!k_rotate_obj) return;
    int32_t scale_q8;
    k_angle = (k_angle + ROTATE_DEMO_STEP) % 360;
    /* Pulse scale 0.5x..1.5x while rotating. */
    scale_q8 = ROTATE_SCALE_BASE + ((sc_sin((int16_t)k_angle) * ROTATE_SCALE_AMP) >> 15);
    if (scale_q8 < 1)
    {
        scale_q8 = 1;
    }
    sc_set_Rotate_scale(k_rotate_obj, scale_q8, scale_q8);
    sc_set_Rotate_angle(k_rotate_obj, k_angle);
    sc_reload_delay(timer, ROTATE_DEMO_MS);
}

void sc_rotate_demo_init(void)
{
    sc_obj_t *screen = sc_page_create(NULL, &k_page, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, 1U);

    k_rotate_obj = sc_Rotate_create(screen, &k_tf, &k_sun_image);
    k_angle = 0;
    sc_set_Rotate_angle(k_rotate_obj, k_angle);
    sc_add_delay(sc_rotate_demo_timer, ROTATE_DEMO_MS, k_rotate_obj);
}

#else

void sc_rotate_demo_init(void)
{
}

#endif
