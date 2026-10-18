# Examples

MCU-portable sample entry files live **only** here: `designer/projects/examples/`.

Every demo declares its own page properties and calls `sc_page_create(NULL, &page_props, ...)` first; a `NULL` parent means the root Screen.

These files do not depend on SDL and do not own `main()` or `sc_gui_loop()`.

## Build / select demo

Edit `app_demo.h` (`APP_DEMO_ID`), or set `APP_DEMO_ID` / `SC_APP_DEMO`, then:

```bat
designer\projects\examples\build_demo.bat
designer\projects\build\scgui.exe
```

Smoke (`APP_DEMO_ID=0`) uses `tests/app_smoke.c` only（仓库内不再保留其它单元测试）。

## Demo assets

Demos 5 / 6 / 8 / 10 / 15 need generated arrays (`sc_demo_assets`, `gif_qoia`) and font `.bin`
files. In the repo they come from `designer/defaults/{generated,fonts}`; exporting examples
copies the complete directory beside `examples/` as `defaults/`.

`APP_DEMO_ID=0` (smoke) is repo-only — `tests/app_smoke.c` is not exported.
