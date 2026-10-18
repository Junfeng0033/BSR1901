@echo off
setlocal EnableDelayedExpansion
rem Hand-maintained: build examples/ demos (does not replace designer-generated build.bat).
rem Select demo: edit examples\app_demo.h (APP_DEMO_ID), or set APP_DEMO_ID / SC_APP_DEMO.
rem All demo sources are compiled; app_demo.c selects which init() to call via #if.
rem Edit SCGUI_SDL2_DIR, or: set SCGUI_SDL2_DIR=C:\path\to\sdl2

for %%I in ("%~dp0..") do set "ROOT=%%~fI"
set EXAMPLES_DIR=%ROOT%\examples
set REPO_ROOT=%ROOT%\..

rem 示例资源随 examples\defaults\ 携带（自包含，无需仓库根回退）。
set "ASSET_DIR=%EXAMPLES_DIR%\defaults\generated"
set "FONT_DIR=%EXAMPLES_DIR%\defaults\fonts"
rem 冒烟测试留在沙箱 tests\app_smoke.c（不随导出工程分发）
set "SMOKE_DIR=%REPO_ROOT%\tests"

if not defined SCGUI_SDL2_DIR (
  set SCGUI_SDL2_DIR=%ROOT%\sdl2
)

if not defined GCC (
  if exist "C:\MinGW\bin\gcc.exe" (set GCC=C:\MinGW\bin\gcc.exe) else (set GCC=gcc)
)

if not exist "%SCGUI_SDL2_DIR%\include\SDL2\SDL.h" (
  echo SDL2 headers not found: %SCGUI_SDL2_DIR%\include\SDL2\SDL.h
  echo Put the SDL2 SDK under designer\projects\sdl2 ^(or set SCGUI_SDL2_DIR^).
  exit /b 1
)

if not exist "%ROOT%\build" mkdir "%ROOT%\build"

rem 可选：环境变量覆盖 app_demo.h 里的 APP_DEMO_ID
set DEMO_DEFINE=
if defined APP_DEMO_ID (
  set "DEMO_DEFINE=-DAPP_DEMO_ID=%APP_DEMO_ID%"
) else if defined SC_APP_DEMO (
  set "DEMO_DEFINE=-DAPP_DEMO_ID=%SC_APP_DEMO%"
)

rem SMOKE demo (ID=0) 需要 tests/app_smoke.c
set SMOKE_SOURCE=
if defined APP_DEMO_ID if "%APP_DEMO_ID%"=="0" (
  if not exist "%SMOKE_DIR%\app_smoke.c" (
    echo APP_DEMO_ID=0 needs tests\app_smoke.c from the scgui repo; pick 1..20 instead.
    exit /b 1
  )
  set SMOKE_SOURCE="%SMOKE_DIR%\app_smoke.c"
)

rem examples\ 不带 sc_config.h；-D_DEMO_CONFIG_ 触发工程根 sc_config.h 的上限分支编译。
set SOURCES="%EXAMPLES_DIR%\demo_main.c" "%ROOT%\sc_sdl2.c" "%EXAMPLES_DIR%\app_demo.c"
for %%F in ("%ROOT%\src\*.c") do set SOURCES=!SOURCES! "%%~F"
for %%F in ("%ROOT%\src\widget\*.c") do set SOURCES=!SOURCES! "%%~F"
for %%F in ("%ROOT%\src\third_party\qrcode\*.c") do set SOURCES=!SOURCES! "%%~F"
rem 编译所有 demo（排除 app_demo.c 自身），未用到的函数由 --gc-sections 优化掉
for %%F in ("%EXAMPLES_DIR%\*_demo.c") do (
  if /I not "%%~nxF"=="app_demo.c" set SOURCES=!SOURCES! "%%~F"
)
if exist "%EXAMPLES_DIR%\demo_collection.c" set SOURCES=!SOURCES! "%EXAMPLES_DIR%\demo_collection.c"
set SOURCES=!SOURCES! !SMOKE_SOURCE!
rem demo 资源
if exist "%ASSET_DIR%\sc_demo_assets.c" set SOURCES=!SOURCES! "%ASSET_DIR%\sc_demo_assets.c"
rem 外部字体开关：=0 时不编译 lv8_font_ex.c（外部字体示例）
if not defined SC_EXT_FONT set SC_EXT_FONT=0
if exist "%FONT_DIR%\font_Arial.c" set SOURCES=!SOURCES! "%FONT_DIR%\font_Arial.c"
if "%SC_EXT_FONT%"=="1" if exist "%FONT_DIR%\lv7_ex_12.c" set SOURCES=!SOURCES! "%FONT_DIR%\lv7_ex_12.c"
if "%SC_EXT_FONT%"=="1" if exist "%FONT_DIR%\lv8_font_ex.c" set SOURCES=!SOURCES! "%FONT_DIR%\lv8_font_ex.c"

echo Building examples with %GCC% ...
"%GCC%" ^
  -std=c99 -Os -Wall -Wno-unused-parameter ^
  -ffunction-sections -fdata-sections ^
  -D_DEMO_CONFIG_ ^
  %DEMO_DEFINE% ^
  -DSC_DEBUG_DEMO_ASSETS=1 ^
  -I"%EXAMPLES_DIR%" ^
  -I"%ROOT%\src" ^
  -I"%ROOT%" ^
  -I"%FONT_DIR%" ^
  -I"%ASSET_DIR%" ^
  -I"%SCGUI_SDL2_DIR%\include" ^
  -L"%SCGUI_SDL2_DIR%\lib" ^
  %SOURCES% ^
  -Wl,--gc-sections ^
  -lmingw32 -lSDL2main -lSDL2 ^
  -o "%ROOT%\build\scgui.exe"
if errorlevel 1 exit /b 1

if exist "%SCGUI_SDL2_DIR%\bin\SDL2.dll" (
  copy /Y "%SCGUI_SDL2_DIR%\bin\SDL2.dll" "%ROOT%\build\SDL2.dll" >nul
)
if exist "%FONT_DIR%\lv7_ex_12.bin" copy /Y "%FONT_DIR%\lv7_ex_12.bin" "%ROOT%\build\" >nul
if exist "%FONT_DIR%\lv7_ex_16.bin" copy /Y "%FONT_DIR%\lv7_ex_16.bin" "%ROOT%\build\" >nul
if exist "%FONT_DIR%\lv8_font_ex.bin" copy /Y "%FONT_DIR%\lv8_font_ex.bin" "%ROOT%\build\" >nul
if exist "%ASSET_DIR%\sc_smoke_flash.bin" copy /Y "%ASSET_DIR%\sc_smoke_flash.bin" "%ROOT%\build\" >nul
if exist "%ASSET_DIR%\gif_qoia.bin" copy /Y "%ASSET_DIR%\gif_qoia.bin" "%ROOT%\build\" >nul
if exist "%ASSET_DIR%\watch.bin" copy /Y "%ASSET_DIR%\watch.bin" "%ROOT%\build\" >nul
echo Build complete: build\scgui.exe
endlocal
