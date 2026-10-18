#include "qcode_demo.h"

#if SC_USER_OBJ && SC_WIDGET_QCODE_ENABLE

#define QCODE_DEMO_SCALE 4U
#define QCODE_DEMO_SIZE \
    ((SC_QCODE_MODULE_COUNT + SC_QCODE_QUIET_ZONE * 2U) * QCODE_DEMO_SCALE)

static sc_container_t k_page;
static sc_qcode_t k_qcode;

void sc_qcode_demo_init(void)
{
    sc_obj_t *screen = sc_page_create(NULL, &k_page, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, 2U);
    sc_obj_t *qcode = sc_qcode_create(screen, &k_qcode);
    sc_obj_set_rect(qcode, 0, 0, QCODE_DEMO_SIZE, QCODE_DEMO_SIZE, ALIGN_CENTER);
    sc_qcode_set_text(qcode, "http://baidu.com");
    sc_invalidate_all();
}

#endif
