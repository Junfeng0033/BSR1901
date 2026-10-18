#include "page_switch_demo.h"
#include "sc_demo_assets.h"
#include "sc_draw.h"

#if SC_USER_OBJ

#define PAGE_COUNT 4U
#define PAGE_X(n_) ((int16_t)((n_) * SC_SCREEN_WIDTH))
#define VOLUME_MS 80U
static sc_container_t k_page;
static sc_obj_t *screen;
static sc_obj_t *k_volume_obj;
static const sc_image_t k_page1_image =
    SC_IMAGE_NEW(img_sun, IMG_SUN_WIDTH, IMG_SUN_HEIGHT, SC_IMAGE_RGB565);
static const sc_image_t k_page2_image =
    SC_IMAGE_NEW(img_wave, IMG_WAVE_WIDTH, IMG_WAVE_HEIGHT, SC_IMAGE_RGB565);
static sc_image_props_t k_page1_wrap = { &k_page1_image, NULL };
static sc_image_props_t k_page2_wrap = { &k_page2_image, NULL };
static sc_label_t k_page1_label;
static sc_label_t k_page3_label;
static sc_text_t k_page3_text_box;
static sc_switch_t k_page2_switch;
static sc_progress_t k_page2_volume;
static sc_msgbox_t k_dyn_msgbox;
static sc_keyboard_t k_page4_keyboard;
static sc_label_t k_page4_value_label;
static sc_obj_t *k_page4_value_obj;
static char k_page4_value[20] = "Value: ";
static uint8_t k_page4_value_len = 7U;
static sc_button_t k_page1_buttons[4];



static const char k_page3_text[] =
    "Text box page demo.\r\n"
    "UP and DOWN scroll this text box.\r\n"
    "Use Prev and Next to switch pages.\r\n"
    "\r\n"
    "Line 01: wrapped text is clipped inside the view.\r\n"
    "Line 02: partial framebuffer drawing stays bounded.\r\n"
    "Line 03: visible-line layout avoids drawing hidden rows.\r\n"
    "Line 04: focus is assigned to this text box.\r\n"
    "Line 05: touch drag scrolls vertically.\r\n"
    "Line 06: page drag still works outside the box.\r\n"
    "Line 07: this page verifies nested widget dispatch.\r\n"
    "Line 08: more content makes scrolling visible.\r\n"
    "Line 09: small GUI code should remain predictable.\r\n"
    "Line 10: demo end.";

static void page_volume_timer(sc_delay_t *timer) {
  if (!k_volume_obj) return;
  if (k_page2_switch.checked) {
    sc_progress_set_value(k_volume_obj,
                          (uint16_t)((k_page2_volume.value + 3U) % 101U));
  }
  sc_reload_delay(timer, VOLUME_MS);
}

static bool page_event_activated(const sc_event_t *event) {
  return event->type == SC_EVENT_TOUCH_UP ||
         (event->type == SC_EVENT_KEY_UP && event->dat.key == CMD_ENTER);
}

static void page_create_msgbox(sc_obj_t *obj, const sc_event_t *event) {
  sc_obj_t *msgbox;
  (void)obj;
  if (!page_event_activated(event) ||
      (g_gui->flags & SC_RT_MSGBOX_ACTIVE) != 0U) {
    return;
  }
  msgbox = sc_msgbox_create(NULL, &k_dyn_msgbox);
  k_dyn_msgbox.flags = SC_MSGBOX_F_DELETE_ON_CLOSE;
  sc_obj_set_text(msgbox, "Confirm action?", SC_FONT_DEFAULT);
  sc_obj_set_rect(msgbox, 48, 84, 224, 112, ALIGN_NONE);
}

static void page_prev_cb(sc_obj_t *obj, const sc_event_t *event) {
  (void)obj;
  if (page_event_activated(event)) {
    sc_page_switch_demo_prev();
  }
}

static void page_next_cb(sc_obj_t *obj, const sc_event_t *event) {
  (void)obj;
  if (page_event_activated(event)) {
    sc_page_switch_demo_next();
  }
}

static void page_root_cb(sc_obj_t *obj, const sc_event_t *event) {
  (void)obj;
  if (event->type != SC_EVENT_KEY_DOWN) {
    return;
  }
  if (event->dat.key == CMD_LEFT) {
    sc_page_switch_demo_prev();
  } else if (event->dat.key == CMD_RIGHT) {
    sc_page_switch_demo_next();
  }
}

static void page_numeric_key(sc_obj_t *obj, uint8_t key) {
  (void)obj;
  if (key >= '0' && key <= '9') {
    if (k_page4_value_len < sizeof(k_page4_value) - 1U) {
      k_page4_value[k_page4_value_len++] = (char)key;
      k_page4_value[k_page4_value_len] = '\0';
    }
  } else if (key == SC_KEYBOARD_KEY_BACKSPACE && k_page4_value_len > 7U) {
    k_page4_value[--k_page4_value_len] = '\0';
  }
  sc_obj_set_text(k_page4_value_obj, k_page4_value, SC_FONT_DEFAULT);
}

void sc_page_switch_demo_init(void) {
  sc_root_set_callback(page_root_cb);
  screen = sc_page_create(NULL, &k_page, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT,
                          PAGE_COUNT);

  sc_obj_t *page1_image = sc_img_create(screen, &k_page1_wrap);
  sc_obj_set_rect(page1_image, PAGE_X(0) + 48, 80, IMG_SUN_WIDTH,
                  IMG_SUN_HEIGHT, ALIGN_NONE);

  sc_obj_t *page1_label = sc_label_create(screen, &k_page1_label);
  sc_obj_set_pos(page1_label, PAGE_X(0) + 24, 48);
  sc_obj_set_text(page1_label, "Page 1", SC_FONT_DEFAULT);
  sc_label_set_x_gap(page1_label, 2U);

  sc_obj_t *page1_button = sc_button_create(screen, &k_page1_buttons[0]);
  sc_obj_set_text(page1_button, "Popup", SC_FONT_DEFAULT);
  sc_obj_set_callback(page1_button, page_create_msgbox);
  sc_obj_set_rect(page1_button, PAGE_X(0) + 24, 152, 88, 32, ALIGN_NONE);

  page1_button = sc_button_create(screen, &k_page1_buttons[1]);
  sc_obj_set_text(page1_button, "Child 2", SC_FONT_DEFAULT);
  sc_obj_set_rect(page1_button, PAGE_X(0) + 24, 192, 88, 32, ALIGN_NONE);

  page1_button = sc_button_create(NULL, &k_page1_buttons[2]);
  sc_obj_set_text(page1_button, "Prev", SC_FONT_DEFAULT);
  sc_obj_set_callback(page1_button, page_prev_cb);
  sc_obj_set_rect(page1_button, 128, 8, 88, 32, ALIGN_NONE);

  page1_button = sc_button_create(NULL, &k_page1_buttons[3]);
  sc_obj_set_text(page1_button, "Next", SC_FONT_DEFAULT);
  sc_obj_set_callback(page1_button, page_next_cb);
  sc_obj_set_rect(page1_button, 224, 8, 88, 32, ALIGN_NONE);

  sc_obj_t *page2_image = sc_img_create(screen, &k_page2_wrap);
  sc_obj_set_rect(page2_image, PAGE_X(1) + 48, 64, IMG_WAVE_WIDTH,
                  IMG_WAVE_HEIGHT, ALIGN_NONE);

  sc_obj_t *page2_switch = sc_switch_create(screen, &k_page2_switch);
  sc_switch_set_checked(page2_switch, true);
  sc_obj_set_rect(page2_switch, PAGE_X(1) + 240, 128, 56, 28, ALIGN_NONE);

  sc_obj_t *page2_volume = sc_progress_create(screen, &k_page2_volume);
  k_volume_obj = page2_volume;
  sc_progress_set_value(page2_volume, 30U);
  sc_obj_set_rect(page2_volume, PAGE_X(1) + 80, 188, 200, 20, ALIGN_NONE);

  sc_obj_t *page3_label = sc_label_create(screen, &k_page3_label);
  sc_obj_set_text(page3_label, "Page 3: Text Box", SC_FONT_DEFAULT);
  sc_obj_set_pos(page3_label, PAGE_X(2) + 24, 48);

  sc_obj_t *page3_text = sc_text_box_create(screen, &k_page3_text_box);
  sc_obj_set_rect(page3_text, PAGE_X(2) + 24, 80, 272, 136, ALIGN_NONE);
  sc_obj_set_text(page3_text, k_page3_text, SC_FONT_DEFAULT);

  k_page4_value_obj = sc_label_create(screen, &k_page4_value_label);
  sc_obj_set_text(k_page4_value_obj, k_page4_value, SC_FONT_DEFAULT);
  sc_obj_set_pos(k_page4_value_obj, PAGE_X(3) + 24, 52);

  sc_obj_t *page4_keyboard = sc_keyboard_create(
      screen, &k_page4_keyboard, SC_KEYBOARD_MODE_NUMBER, page_numeric_key);
  sc_obj_set_rect(page4_keyboard, PAGE_X(3) + 28, 80, 264, 144, ALIGN_NONE);

  sc_invalidate_all();
  sc_add_delay(page_volume_timer, VOLUME_MS, NULL);
}

void sc_page_switch_demo_next(void) {
  sc_container_page_by_key(screen, 1, true);
}

void sc_page_switch_demo_prev(void) {
  sc_container_page_by_key(screen, -1, true);
}

#endif
