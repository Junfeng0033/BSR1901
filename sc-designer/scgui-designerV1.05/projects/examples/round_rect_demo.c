#include "round_rect_demo.h"

#include "sc_demo_assets.h"

#define TEXTURE_W 120U
#define TEXTURE_H 72U

static sc_container_t k_page;
static sc_texture_t k_hor_texture;
static sc_texture_t k_ver_texture;
static sc_texture_t k_center_texture;
static sc_texture_t k_radial_texture;
static sc_texture_t k_title_texture;

static sc_rounded_t k_cards[4];
static sc_label_t k_labels[5];

static void sc_demo_create_card(sc_obj_t *screen, sc_rounded_t *props,
                                sc_texture_t *texture, sc_grad_dir_t dir,
                                color_t c0, color_t c1,
                                int16_t x, int16_t y)
{
    sc_obj_t *obj = sc_round_rect_create(screen, props);
    props->radius = 16U;
    props->border = 2U;
    props->rect_mask = 1U;
    sc_obj_set_rect(obj, x, y, TEXTURE_W, TEXTURE_H, ALIGN_NONE);
    sc_obj_add_texture(obj, texture, dir, c0, c1);
}

static void sc_demo_create_label(sc_obj_t *screen, sc_label_t *props,
                                 const char *text, int16_t x, int16_t y)
{
    sc_obj_t *obj = sc_label_create(screen, props);
    sc_obj_set_text(obj, text, SC_FONT_DEFAULT);
    sc_obj_set_pos(obj, x, y);
}

void sc_round_rect_demo_init(void)
{
    sc_obj_t *screen = sc_page_create(NULL, &k_page, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, 1U);

    sc_obj_t *title = sc_label_create(screen, &k_labels[0]);
    sc_obj_set_text(title, "Rounded texture: Center vs Radial", SC_FONT_DEFAULT);
    sc_obj_add_texture(title, &k_title_texture, SC_GRAD_HOR,
                       C_DODGER_BLUE, C_MAGENTA);
    sc_obj_set_pos(title, 12, 5);

    sc_demo_create_card(screen, &k_cards[0], &k_hor_texture, SC_GRAD_HOR,
                        C_DODGER_BLUE, C_MAGENTA, 20, 28);
    sc_demo_create_label(screen, &k_labels[1], "Horizontal", 20, 104);

    sc_demo_create_card(screen, &k_cards[1], &k_ver_texture, SC_GRAD_VER,
                        C_GREEN, C_YELLOW, 180, 28);
    sc_demo_create_label(screen, &k_labels[2], "Vertical", 180, 104);

    /* Legacy Manhattan diamond vs LED-style circular spread. */
    sc_demo_create_card(screen, &k_cards[2], &k_center_texture, SC_GRAD_CENTER,
                        C_ORANGE, C_BLUE, 20, 136);
    sc_demo_create_label(screen, &k_labels[3], "Center (old)", 20, 212);

    sc_demo_create_card(screen, &k_cards[3], &k_radial_texture, SC_GRAD_RADIAL,
                        C_ORANGE, C_BLUE, 180, 136);
    sc_demo_create_label(screen, &k_labels[4], "Radial (w+h)/2", 180, 212);

    sc_invalidate_all();
}
