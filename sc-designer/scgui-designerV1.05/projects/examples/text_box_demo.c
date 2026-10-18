#include "text_box_demo.h"

#if SC_USER_OBJ

static sc_container_t k_page;
static sc_text_t k_demo_text_box_props;

static const char k_demo_text[] =
    "SCGUI text box demo.\r\n"
    "Use UP and DOWN keys to scroll 5 pixels.\r\n"
    "\r\n"
    "This widget keeps only the visible view on screen.\r\n"
    "Each wrapped line is centered inside the view.\r\n"
    "offs_y controls vertical movement for ebook style reading.\r\n"
    "\r\n"
    "Line 01: Embedded GUI needs tight RAM control.\r\n"
    "Line 02: Text layout should be predictable.\r\n"
    "Line 03: Partial framebuffer keeps memory low.\r\n"
    "Line 04: SDL here is only for simulator testing.\r\n"
    "Line 05: Touch is optional for this demo.\r\n"
    "Line 06: Keyboard scrolling helps quick verification.\r\n"
    "Line 07: The box clips content to its own rect.\r\n"
    "Line 08: Long text can continue far beyond one screen.\r\n"
    "Line 09: This line exists to extend the sample.\r\n"
    "Line 10: More lines make scrolling easier to observe.\r\n"
    "Line 11: Centering is applied per visible line.\r\n"
    "Line 12: offs_y can also be animated externally.\r\n"
    "Line 13: Selection defaults to the text box in demo.\r\n"
    "Line 14: Up key moves text downward.\r\n"
    "Line 15: Down key moves text upward.\r\n"
    "Line 16: This should exceed the visible area.\r\n"
    "Line 17: Continue reading the sample content.\r\n"
    "Line 18: Small systems still benefit from good UX.\r\n"
    "Line 19: The end is close.\r\n"
    "Line 20: Demo end.";

void sc_text_box_demo_init(void)
{
    sc_obj_t *screen = sc_page_create(NULL, &k_page, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, 1U);
    sc_obj_t *obj = sc_text_box_create(screen, &k_demo_text_box_props);
    sc_obj_set_rect(obj, 24, 92, 272, 132, ALIGN_NONE);
    sc_obj_set_text(obj, k_demo_text, SC_FONT_DEFAULT);
    sc_obj_set_key_focus(obj);
}

#endif /* SC_USER_OBJ */
