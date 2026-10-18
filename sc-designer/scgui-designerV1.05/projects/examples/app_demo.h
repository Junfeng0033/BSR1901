#ifndef APP_DEMO_H
#define APP_DEMO_H

#define APP_DEMO_SMOKE 0U
#define APP_DEMO_CONTAINER 1U
#define APP_DEMO_LINE_EDIT 2U
#define APP_DEMO_KEYBOARD 3U
#define APP_DEMO_QCODE 4U
#define APP_DEMO_PAGE_SWITCH 5U
#define APP_DEMO_ROUND_RECT 6U
#define APP_DEMO_LABEL 7U
#define APP_DEMO_COLLECTION 8U
#define APP_DEMO_TEXT_BOX 9U
#define APP_DEMO_IMAGE 10U
#define APP_DEMO_GIF 11U
#define APP_DEMO_ARC 12U
#define APP_DEMO_LINE 13U
#define APP_DEMO_WAVEFORM 14U
#define APP_DEMO_ROTATE 15U
#define APP_DEMO_LIST_ITEMS 16U
#define APP_DEMO_TRANSFORM 17U
#define APP_DEMO_TRIANGLE 18U
#define APP_DEMO_EYE 19U
#define APP_DEMO_NUMBER 20U
#define APP_DEMO_DROPDOWN 21U
#define APP_DEMO_SEGMENT_SWITCH 22U
#define APP_DEMO_ANIM 23U
#define APP_DEMO_DROPDOWN_CARD 24U
#define APP_DEMO_LIST_MENU 25U
#define APP_DEMO_COUNT 26U

/*
 * User demo selection: edit APP_DEMO_ID only.
 * 0=SMOKE runs tests/app_smoke.c → app_smoke_test(); 1..16 select the matching demo.
 * build_demo.bat follows this header unless APP_DEMO_ID / SC_APP_DEMO is already
 * set in the environment (e.g. by run_*.bat).
 */
#ifndef APP_DEMO_ID
#define APP_DEMO_ID APP_DEMO_IMAGE
#endif

#ifndef SC_APP_DEMO
#define SC_APP_DEMO APP_DEMO_ID
#endif

#if APP_DEMO_ID >= APP_DEMO_COUNT
#error APP_DEMO_ID is outside the supported demo range
#endif

void app_demo_init(void);

#endif
