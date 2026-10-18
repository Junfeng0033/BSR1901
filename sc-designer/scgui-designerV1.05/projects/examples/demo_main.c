/* Demo build host main — self-contained, no app_ui.h / app_ui_examples.c dependency.
 * Project builds (build.bat) use projects/main.c; hand-written demos use this. */
#include "sc_sdl2.h"
#include "app_demo.h"
#include "font_Arial.h"
#include "widget/sc_eye.h"

#if APP_DEMO_ID == APP_DEMO_SMOKE
int app_smoke_test(void);
#endif

int main(void)
{
    sc_sdl2_init();
    sc_gui_init(sc_sdl2_lcd_flush, NULL, 0x0000U);
    sc_gui_add_font(SC_FONT_DEFAULT, &font_Arial, SC_FONT_INTERNAL);
#if APP_DEMO_ID == APP_DEMO_SMOKE
    sc_sdl2_quit(app_smoke_test());
    return sc_sdl2_exit_code();
#else
    app_demo_init();
#endif
    while (sc_sdl2_loop())
    {
        sc_gui_loop();
    }
    sc_sdl2_close();
    return sc_sdl2_exit_code();
}
