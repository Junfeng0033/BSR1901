#include "line_demo.h"

#if SC_USER_OBJ && SC_WIDGET_LINE_ENABLE

static sc_container_t k_page;
static sc_line_t k_line_thin;
static sc_line_t k_line_thick;
static sc_line_t k_line_round;
static sc_line_t k_line_diag;
static sc_label_t k_title;
static sc_label_t k_hint;

void sc_line_demo_init(void)
{
    sc_obj_t *screen = sc_page_create(NULL, &k_page, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, 1U);

    sc_obj_t *title = sc_label_create(screen, &k_title);
    sc_obj_set_text(title, "Line demo", SC_FONT_DEFAULT);
    sc_obj_set_pos(title, 12, 8);

    sc_obj_t *line_thin = sc_line_create(screen, &k_line_thin);
    k_line_thin.color = C_CYAN;
    k_line_thin.stroke_width = 1U;
    k_line_thin.round_cap = 0U;
    sc_line_set_points(line_thin, 16, 48, SC_SCREEN_WIDTH - 16, 48);

    sc_obj_t *line_thick = sc_line_create(screen, &k_line_thick);
    k_line_thick.color = C_YELLOW;
    k_line_thick.stroke_width = 8U;
    k_line_thick.round_cap = 0U;
    sc_line_set_points(line_thick, 16, 88, SC_SCREEN_WIDTH - 16, 88);

    sc_obj_t *line_round = sc_line_create(screen, &k_line_round);
    k_line_round.color = C_ORANGE;
    k_line_round.stroke_width = 10U;
    k_line_round.round_cap = 1U;
    sc_line_set_points(line_round, 8, 50, SC_SCREEN_WIDTH - 80, 140);

    sc_obj_t *line_diag = sc_line_create(screen, &k_line_diag);
    k_line_diag.color = C_LIME;
    k_line_diag.stroke_width = 6U;
    k_line_diag.round_cap = 2U;
    sc_line_set_points(line_diag, 24, 190, SC_SCREEN_WIDTH - 24, SC_SCREEN_HEIGHT - 40);

    sc_obj_t *hint = sc_label_create(screen, &k_hint);
    sc_obj_set_text(hint, "thin / thick / round cap / diagonal", SC_FONT_DEFAULT);
    sc_obj_set_pos(hint, 12, SC_SCREEN_HEIGHT - 28);
}

#endif /* SC_USER_OBJ && SC_WIDGET_LINE_ENABLE */
