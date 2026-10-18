#include "app_demo.h"

#include "sc_gui.h"

#if SC_USER_OBJ
#if APP_DEMO_ID == APP_DEMO_CONTAINER
#include "container_demo.h"
#elif APP_DEMO_ID == APP_DEMO_LINE_EDIT
#include "line_edit_demo.h"
#elif APP_DEMO_ID == APP_DEMO_KEYBOARD
#include "keyboard_demo.h"
#elif APP_DEMO_ID == APP_DEMO_QCODE
#include "qcode_demo.h"
#elif APP_DEMO_ID == APP_DEMO_PAGE_SWITCH
#include "page_switch_demo.h"
#elif APP_DEMO_ID == APP_DEMO_ROUND_RECT
#include "round_rect_demo.h"
#elif APP_DEMO_ID == APP_DEMO_LABEL
#include "label_demo.h"
#elif APP_DEMO_ID == APP_DEMO_TEXT_BOX
#include "text_box_demo.h"
#elif APP_DEMO_ID == APP_DEMO_COLLECTION
#include "demo_collection.h"
#elif APP_DEMO_ID == APP_DEMO_IMAGE
#include "image_demo.h"
#elif APP_DEMO_ID == APP_DEMO_GIF
#include "gif_demo.h"
#elif APP_DEMO_ID == APP_DEMO_ARC
#include "arc_demo.h"
#elif APP_DEMO_ID == APP_DEMO_LINE
#include "line_demo.h"
#elif APP_DEMO_ID == APP_DEMO_WAVEFORM
#include "waveform_demo.h"
#elif APP_DEMO_ID == APP_DEMO_ROTATE
#include "rotate_demo.h"
#elif APP_DEMO_ID == APP_DEMO_LIST_ITEMS
#include "sc_list_items_demo.h"
#elif APP_DEMO_ID == APP_DEMO_TRANSFORM
#include "transform_demo.h"
#elif APP_DEMO_ID == APP_DEMO_TRIANGLE
#include "triangle_demo.h"
#elif APP_DEMO_ID == APP_DEMO_EYE
#include "eye_demo.h"
#elif APP_DEMO_ID == APP_DEMO_NUMBER
#include "number_demo.h"
#elif APP_DEMO_ID == APP_DEMO_DROPDOWN
#include "dropdown_demo.h"
#elif APP_DEMO_ID == APP_DEMO_SEGMENT_SWITCH
#include "segment_switch_demo.h"
#elif APP_DEMO_ID == APP_DEMO_ANIM
#include "anim_demo.h"
#elif APP_DEMO_ID == APP_DEMO_DROPDOWN_CARD
#include "dropdown_card_demo.h"
#elif APP_DEMO_ID == APP_DEMO_LIST_MENU
#include "sc_list_menu_demo.h"
#endif
#endif

void app_demo_init(void)
{
#if SC_USER_OBJ
#if APP_DEMO_ID == APP_DEMO_CONTAINER
    sc_container_demo_init();
#elif APP_DEMO_ID == APP_DEMO_LINE_EDIT
    sc_line_edit_demo_init();
#elif APP_DEMO_ID == APP_DEMO_KEYBOARD
    sc_keyboard_demo_init();
#elif APP_DEMO_ID == APP_DEMO_QCODE && SC_WIDGET_QCODE_ENABLE
    sc_qcode_demo_init();
#elif APP_DEMO_ID == APP_DEMO_PAGE_SWITCH
    sc_page_switch_demo_init();
#elif APP_DEMO_ID == APP_DEMO_ROUND_RECT
    sc_round_rect_demo_init();
#elif APP_DEMO_ID == APP_DEMO_LABEL
    sc_label_demo_init();
#elif APP_DEMO_ID == APP_DEMO_TEXT_BOX
    sc_text_box_demo_init();
#elif APP_DEMO_ID == APP_DEMO_COLLECTION
    sc_demo_collection_init();
#elif APP_DEMO_ID == APP_DEMO_IMAGE
    sc_image_demo_init();
#elif APP_DEMO_ID == APP_DEMO_GIF
    sc_gif_demo_init();
#elif APP_DEMO_ID == APP_DEMO_ARC && SC_WIDGET_ARC_ENABLE
    sc_arc_demo_init();
#elif APP_DEMO_ID == APP_DEMO_LINE && SC_WIDGET_LINE_ENABLE
    sc_line_demo_init();
#elif APP_DEMO_ID == APP_DEMO_WAVEFORM && SC_WIDGET_WAVEFORM_ENABLE
    sc_waveform_demo_init();
#elif APP_DEMO_ID == APP_DEMO_ROTATE && SC_WIDGET_TRANSFORM_ENABLE
    sc_rotate_demo_init();
#elif APP_DEMO_ID == APP_DEMO_LIST_ITEMS
    sc_list_items_demo_init();
#elif APP_DEMO_ID == APP_DEMO_TRANSFORM && SC_WIDGET_TRANSFORM_ENABLE
    sc_transform_demo_init();
#elif APP_DEMO_ID == APP_DEMO_TRIANGLE && SC_WIDGET_TRIANGLE_ENABLE
    sc_triangle_demo_init();
#elif APP_DEMO_ID == APP_DEMO_EYE && SC_WIDGET_EYE_ENABLE
    sc_eye_demo_init();
#elif APP_DEMO_ID == APP_DEMO_NUMBER && SC_WIDGET_NUMBER_ENABLE
    sc_number_demo_init();
#elif APP_DEMO_ID == APP_DEMO_DROPDOWN && SC_WIDGET_DROPDOWN_ENABLE
    sc_dropdown_demo_init();
#elif APP_DEMO_ID == APP_DEMO_SEGMENT_SWITCH && SC_WIDGET_SEGMENT_SWITCH_ENABLE
    sc_segment_switch_demo_init();
#elif APP_DEMO_ID == APP_DEMO_ANIM
    anim_demo_init();
#elif APP_DEMO_ID == APP_DEMO_DROPDOWN_CARD
    dropdown_card_demo_init();
#elif APP_DEMO_ID == APP_DEMO_LIST_MENU && SC_WIDGET_LIST_MENU_ENABLE
    sc_list_menu_demo_init();
#endif
#endif
}
