#!/usr/bin/env bash
# Hand-maintained: build examples/ demos (does not replace designer-generated build.sh).
# Select demo: edit examples/app_demo.h (APP_DEMO_ID), or export APP_DEMO_ID / SC_APP_DEMO.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
EXAMPLES_DIR="$ROOT/examples"
# tauri_pro 沙箱自包含：REPO_ROOT 即工程根（旧仓库是 ROOT/../.. 且 defaults 在 designer/ 下）
REPO_ROOT="$(cd "$ROOT/.." && pwd)"
# Exported projects carry defaults/ beside examples/; repo builds fall back to sandbox defaults/.
ASSET_DIR="$ROOT/defaults/generated"
[[ -f "$ASSET_DIR/sc_demo_assets.c" ]] || ASSET_DIR="$REPO_ROOT/defaults/generated"
FONT_DIR="$ROOT/defaults/fonts"
[[ -f "$FONT_DIR/font_Arial.c" ]] || FONT_DIR="$REPO_ROOT/defaults/fonts"
# 冒烟测试留在沙箱 tests/app_smoke.c（不随导出工程分发）
SMOKE_DIR="$REPO_ROOT/tests"
SCGUI_SDL2_DIR="${SCGUI_SDL2_DIR:-$ROOT/sdl2}"
if [[ ! -f "$SCGUI_SDL2_DIR/include/SDL2/SDL.h" ]]; then
  echo "SDL2 headers not found: $SCGUI_SDL2_DIR/include/SDL2/SDL.h" >&2
  echo "Put the SDL2 SDK under designer/projects/sdl2 (or set SCGUI_SDL2_DIR)." >&2
  exit 1
fi
CC="${CC:-gcc}"
OUT_DIR="$ROOT/build"
OUT="$OUT_DIR/scgui"
PROBE_OUT="$OUT_DIR/app_demo_id.txt"

mkdir -p "$OUT_DIR"

DEMO_DEFINE=()
ASSET_DEFINE=()
if [[ -n "${APP_DEMO_ID:-}" ]]; then
  DEMO_ID="$APP_DEMO_ID"
  DEMO_DEFINE=(-DAPP_DEMO_ID="$APP_DEMO_ID")
elif [[ -n "${SC_APP_DEMO:-}" ]]; then
  DEMO_ID="$SC_APP_DEMO"
  DEMO_DEFINE=(-DAPP_DEMO_ID="$SC_APP_DEMO")
else
  "$CC" -E -P -I"$ROOT/src" -I"$EXAMPLES_DIR" "$EXAMPLES_DIR/app_demo_id_probe.c" >"$PROBE_OUT" 2>/dev/null || true
  DEMO_ID="$(awk '/APP_DEMO_ID_VALUE/{print $2; exit}' "$PROBE_OUT" 2>/dev/null || true)"
fi
DEMO_ID="${DEMO_ID%U}"
if [[ -z "${DEMO_ID:-}" ]]; then
  echo "Could not resolve APP_DEMO_ID from examples/app_demo.h" >&2
  exit 1
fi

DEMO_SOURCES=()
EXTRA_ASSETS=()
SMOKE_SOURCE=()
case "$DEMO_ID" in
  0)
    if [[ ! -f "$SMOKE_DIR/app_smoke.c" ]]; then
      echo "APP_DEMO_ID=0 needs tests/app_smoke.c from the scgui repo; pick 1..16 instead." >&2
      exit 1
    fi
    SMOKE_SOURCE=("$SMOKE_DIR/app_smoke.c")
    ;;
  1) DEMO_SOURCES=("$EXAMPLES_DIR/container_demo.c") ;;
  2) DEMO_SOURCES=("$EXAMPLES_DIR/line_edit_demo.c") ;;
  3) DEMO_SOURCES=("$EXAMPLES_DIR/keyboard_demo.c") ;;
  4) DEMO_SOURCES=("$EXAMPLES_DIR/qcode_demo.c") ;;
  5) DEMO_SOURCES=("$EXAMPLES_DIR/page_switch_demo.c"); ASSET_DEFINE=(-DSC_DEBUG_DEMO_ASSETS=1) ;;
  6) DEMO_SOURCES=("$EXAMPLES_DIR/round_rect_demo.c"); ASSET_DEFINE=(-DSC_DEBUG_DEMO_ASSETS=1) ;;
  7) DEMO_SOURCES=("$EXAMPLES_DIR/label_demo.c") ;;
  8)
    DEMO_SOURCES=(
      "$EXAMPLES_DIR/demo_collection.c"
      "$EXAMPLES_DIR/container_demo.c"
      "$EXAMPLES_DIR/label_demo.c"
      "$EXAMPLES_DIR/line_edit_demo.c"
      "$EXAMPLES_DIR/round_rect_demo.c"
      "$EXAMPLES_DIR/text_box_demo.c"
    )
    ASSET_DEFINE=(-DSC_DEBUG_DEMO_ASSETS=1)
    ;;
  9) DEMO_SOURCES=("$EXAMPLES_DIR/text_box_demo.c") ;;
  10)
    DEMO_SOURCES=("$EXAMPLES_DIR/image_demo.c")
    EXTRA_ASSETS=()
    ;;
  12) DEMO_SOURCES=("$EXAMPLES_DIR/arc_demo.c") ;;
  13) DEMO_SOURCES=("$EXAMPLES_DIR/line_demo.c") ;;
  14) DEMO_SOURCES=("$EXAMPLES_DIR/waveform_demo.c") ;;
  15) DEMO_SOURCES=("$EXAMPLES_DIR/rotate_demo.c"); ASSET_DEFINE=(-DSC_DEBUG_DEMO_ASSETS=1) ;;
  16) DEMO_SOURCES=("$EXAMPLES_DIR/sc_list_items_demo.c") ;;
  17) DEMO_SOURCES=("$EXAMPLES_DIR/transform_demo.c"); ASSET_DEFINE=(-DSC_DEBUG_DEMO_ASSETS=1) ;;
  18) DEMO_SOURCES=("$EXAMPLES_DIR/triangle_demo.c") ;;
  19) DEMO_SOURCES=("$EXAMPLES_DIR/eye_demo.c") ;;
  *)
    echo "Invalid APP_DEMO_ID=$DEMO_ID; expected 0 through 19." >&2
    exit 1
    ;;
esac

if [[ "$DEMO_ID" != "0" && -f "$ASSET_DIR/sc_demo_assets.c" ]]; then
  EXTRA_ASSETS+=("$ASSET_DIR/sc_demo_assets.c")
fi

shopt -s nullglob
# Demo build uses examples/demo_main.c (not the designer-generated main.c) so the
# exported project sc_config.h is never pulled in; demos fall back to src/sc_config.h.
SOURCES=("$EXAMPLES_DIR/demo_main.c" "$ROOT/sc_sdl2.c" "$EXAMPLES_DIR/app_ui_examples.c" "$EXAMPLES_DIR/app_demo.c")
SOURCES+=("$ROOT"/src/*.c)
SOURCES+=("$ROOT"/src/widget/*.c)
SOURCES+=("$ROOT"/src/third_party/qrcode/*.c)
SOURCES+=("$ROOT"/images/*.c)
SOURCES+=("$ROOT"/fonts/*.c)
SOURCES+=("${DEMO_SOURCES[@]}")
SOURCES+=("${SMOKE_SOURCE[@]}")
SOURCES+=("${EXTRA_ASSETS[@]}")
if [[ -f "$FONT_DIR/font_Arial.c" ]]; then SOURCES+=("$FONT_DIR/font_Arial.c"); fi
if [[ -f "$FONT_DIR/lv7_ex_12.c" ]]; then SOURCES+=("$FONT_DIR/lv7_ex_12.c"); fi
if [[ -f "$FONT_DIR/lv8_font_ex.c" ]]; then SOURCES+=("$FONT_DIR/lv8_font_ex.c"); fi

echo "Building examples APP_DEMO_ID=$DEMO_ID with $CC ..."
"$CC" \
  -std=c99 -O2 -Wall -Wno-unused-parameter \
  -ffunction-sections -fdata-sections \
  "${DEMO_DEFINE[@]}" \
  "${ASSET_DEFINE[@]}" \
  -DSC_WIDGET_IMAGE_ENABLE=1 \
  -DSC_WIDGET_TEXT_BOX_ENABLE=1 \
  -DSC_WIDGET_MSGBOX_ENABLE=1 \
  -DSC_WIDGET_COMPOSITE_ENABLE=1 \
  -DSC_WIDGET_KEYBOARD_ENABLE=1 \
  -DSC_WIDGET_LINE_EDIT_ENABLE=1 \
  -DSC_WIDGET_QCODE_ENABLE=1 \
  -DSC_WIDGET_TRANSFORM_ENABLE=1 \
  -I"$ROOT/src" \
  -I"$ROOT" \
  -I"$ROOT/images" \
  -I"$ROOT/fonts" \
  -I"$EXAMPLES_DIR" \
  -I"$FONT_DIR" \
  -I"$ASSET_DIR" \
  -I"$SCGUI_SDL2_DIR/include" \
  -L"$SCGUI_SDL2_DIR/lib" \
  "${SOURCES[@]}" \
  -Wl,--gc-sections \
  -lSDL2main -lSDL2 \
  -o "$OUT"

if [[ -f "$SCGUI_SDL2_DIR/bin/SDL2.dll" ]]; then
  cp -f "$SCGUI_SDL2_DIR/bin/SDL2.dll" "$OUT_DIR/SDL2.dll"
fi
if [[ -f "$ASSET_DIR/gif_qoia.bin" ]]; then
  cp -f "$ASSET_DIR/gif_qoia.bin" "$OUT_DIR/gif_qoia.bin"
fi
for bin in lv7_ex_12.bin lv7_ex_16.bin lv8_font_ex.bin; do
  if [[ -f "$FONT_DIR/$bin" ]]; then cp -f "$FONT_DIR/$bin" "$OUT_DIR/$bin"; fi
done
echo "Build complete: $OUT"
