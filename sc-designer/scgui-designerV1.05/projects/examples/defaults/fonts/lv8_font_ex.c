#include "sc_lvgl_font.h"

/* Headerless SCF8 metadata and glyph cache are shared by scgui. */
#if !SC_EXTERNAL_FONT_ENABLE
#error "SCF8 requires SC_EXTERNAL_FONT_ENABLE"
#endif
#if SC_EXTERNAL_FONT_GLYPH_BUFFER_SIZE < 210U
#error "SC_EXTERNAL_FONT_GLYPH_BUFFER_SIZE is too small for this font"
#endif

static const sc_ext_font_range_t __user_font_ranges[] = {
    {0x00000020U, 0x0000007EU, 0U},
};

static const sc_ext_font_scf8_t __user_font_dsc = {
    .ranges = __user_font_ranges,
    .file_size = 9475U,
    .glyph_count = 95U,
    .range_count = 1U,
    .bpp = 4U,
};

// lv8_font_ex,22 (scgui / headerless SCF8 external)
const lv_font_t lv8_font_ex = {
    .get_glyph_bitmap = sc_ext_font_v8_get_bitmap,
    .get_glyph_dsc = sc_ext_font_v8_get_glyph_dsc,
    .line_height = 25,
    .base_line = 6,
    .dsc = &__user_font_dsc,
};
