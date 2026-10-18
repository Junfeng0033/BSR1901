#include "transform_demo.h"

#include "sc_demo_assets.h"

#if SC_USER_OBJ && SC_WIDGET_TRANSFORM_ENABLE

#define TF_DEMO_MS 30U

static sc_container_t k_page;
static Transform_t k_tf;
static sc_obj_t *k_tf_obj;
static const sc_image_t k_sun =
    SC_IMAGE_NEW(img_sun, IMG_SUN_WIDTH, IMG_SUN_HEIGHT, SC_IMAGE_RGB565);
static int16_t k_angle;

static void sc_transform_demo_timer(sc_delay_t *timer)
{
    if (!k_tf_obj)
    {
        return;
    }
    k_angle = (int16_t)((k_angle + 3) % 360);
    sc_set_Rotate_angle(k_tf_obj, k_angle);
    sc_reload_delay(timer, TF_DEMO_MS);
}

void sc_transform_demo_init(void)
{
    sc_obj_t *screen = sc_page_create(NULL, &k_page, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, 1U);
    if (!screen)
    {
        return;
    }

    k_tf_obj = sc_Rotate_create(screen, &k_tf, &k_sun);
    if (!k_tf_obj)
    {
        return;
    }
    sc_add_delay(sc_transform_demo_timer, TF_DEMO_MS, k_tf_obj);
}

#elif SC_USER_OBJ

void sc_transform_demo_init(void)
{
}

#endif
