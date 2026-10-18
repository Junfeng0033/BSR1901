#include "eye_demo.h"

#if SC_USER_OBJ && SC_WIDGET_EYE_ENABLE

#define EYE_IMG_W 160U     /* 眼图宽（> 眼视图宽，两侧留平移余量） */
#define EYE_IMG_H 80U
#define IRIS_CX 80          /* 眼珠中心在图片内的 x */
#define IRIS_CY 40          /* 眼珠中心在图片内的 y */
#define IRIS_X_SHIFT 10U    /* 眨眼扫视时虹膜横向最大位移 */

/* 纹理基础偏移：让眼图相对眼视图（w=110 → 宽 116，上38+下30 → 高 74）居中，
 * 两侧各留 (160-116)/2=22px 余量 → 左右平移不越界出黑（越界取样返回 0=黑）。
 * 平移时写 k_iris_texture.offset_x = IRIS_BASE_OFF_X + 扫视偏移。 */
#define IRIS_BASE_OFF_X (-(int16_t)((EYE_IMG_W - 116U) / 2U))
#define IRIS_BASE_OFF_Y (-(int16_t)((EYE_IMG_H - 74U) / 2U))

/* 动画定时模板：单一相位变量 s_anim(0..ANIM_TOTAL-1) 驱动两个输出——
 * 扫视段虹膜三角波往返、眨眼段深度 0→255→0，均为纯函数逐帧推导（前端预览
 * JS 可照搬同一套公式）。每步 BLINK_STEP_MS(16ms)。 */
#define BLINK_STEP_MS  16U
#define ANIM_SCAN      64U  /* 扫视步数：虹膜 0→+10→0→-10→0（周期边界连续） */
#define ANIM_BLINK     32U  /* 眨眼步数：闭 16 + 开 16 */
#define ANIM_TOTAL     (ANIM_SCAN + ANIM_BLINK)

static sc_container_t k_page;
static sc_label_t k_title;
static sc_label_t k_hint;
static sc_eye_t k_eye_l;
static sc_eye_t k_eye_r;
static sc_texture_t k_iris_texture; /* 两只眼共用同一张纹理 */
static color_t k_iris_pixels[EYE_IMG_W * EYE_IMG_H];
static const sc_image_t k_iris_image =
    {.data.pixels = k_iris_pixels, .width = EYE_IMG_W, .height = EYE_IMG_H, .type = SC_IMAGE_RGB565};

/* 双眼对象 + 动画状态（单一驱动变量）。 */
static sc_obj_t *k_eye_l_obj;
static sc_obj_t *k_eye_r_obj;
static uint8_t s_anim; /* 周期相位 0..ANIM_TOTAL-1，驱动平移与眨眼 */

/* 虹膜横向偏移（相对居中基础偏移）：三角波 0→+10→0→-10→0，周期边界连续。 */
static int16_t sc_eye_anim_iris(uint8_t t)
{
    const int16_t amp = (int16_t)IRIS_X_SHIFT;
    const int16_t v = (int16_t)(t & 15U) * amp / 15;
    return (t < 32U) ? ((t < 16U) ? v : amp - v)
                     : ((t < 48U) ? -v : v - amp);
}

/* 眨眼深度（blink 语义 0=闭眼、255=最大开眼）：闭眼段 255→0，睁眼段 0→255
 * （扫视段外部才调用，扫视段睁眼 blink=255）。 */
static uint8_t sc_eye_anim_blink(uint8_t t)
{
    const uint8_t b = (uint8_t)(t - ANIM_SCAN);
    return (b < 16U) ? (uint8_t)(255U - b * 255U / 15U)
                     : (uint8_t)((b - 16U) * 255U / 15U);
}

static void sc_eye_demo_timer(sc_delay_t *timer)
{
    if (!k_eye_l_obj || !k_eye_r_obj) return;
    const uint8_t t = s_anim;
    /* 扫视段：睁眼 + 虹膜往返；眨眼段：深度 255→0→255（闭→开），虹膜停中心。 */
    k_iris_texture.offset_x = (int16_t)(IRIS_BASE_OFF_X +
                                        ((t < ANIM_SCAN) ? sc_eye_anim_iris(t) : 0));
    sc_obj_invalidate(k_eye_l_obj);
    sc_obj_invalidate(k_eye_r_obj);
    const uint8_t blink = (t < ANIM_SCAN) ? 255U : sc_eye_anim_blink(t);
    sc_eye_set_blink(k_eye_l_obj, blink);
    sc_eye_set_blink(k_eye_r_obj, blink);
    s_anim = (uint8_t)((s_anim + 1U) % ANIM_TOTAL);
    sc_reload_delay(timer, BLINK_STEP_MS);
}

/* 生成内联 RGB565 眼图：瞳孔 + 虹膜渐变 + 巩膜，作为图片纹理被杏仁 mask 裁剪。 */
static void sc_eye_demo_build_iris(void)
{
    const int cx = IRIS_CX;
    const int cy = IRIS_CY;
    for (int y = 0; y < (int)EYE_IMG_H; y++)
    {
        for (int x = 0; x < (int)EYE_IMG_W; x++)
        {
            uint32_t d = sc_isqrt_u32((uint32_t)((x - cx) * (x - cx)) + (uint32_t)((y - cy) * (y - cy)));
            color_t c;
            if (d < 9U)
            {
                c = C_BLACK;                       /* 瞳孔 */
            }
            else if (d < 26U)
            {
                c = alphaBlend(C_BLUE, C_BROWN, (uint8_t)(((d - 9U) * 255U) / 17U)); /* 虹膜棕→蓝 */
            }
            else
            {
                c = C_WHITE;                       /* 巩膜 */
            }
            k_iris_pixels[(uint32_t)y * EYE_IMG_W + (uint32_t)x] = c;
        }
    }
}

void sc_eye_demo_init(void)
{
    sc_eye_demo_build_iris();

    sc_obj_t *screen = sc_page_create(NULL, &k_page, SC_SCREEN_WIDTH, SC_SCREEN_HEIGHT, 1U);

    sc_obj_t *title = sc_label_create(screen, &k_title);
    sc_obj_set_text(title, "Eye demo: symmetric pair, shared iris texture", SC_FONT_DEFAULT);
    sc_obj_set_pos(title, 12, 8);

    /* 左右对称两只眼：同形状同描边，共用 k_iris_texture（各按自己 view 原点取样，
     * 杏仁 mask 内显示同一张眼图）。 */
    sc_obj_t *eye_l = sc_eye_create(screen, &k_eye_l);
    k_eye_l.color = C_WHITE;
    k_eye_l.fill = C_RED; /* 有纹理时以纹理为准 */
    k_eye_l.stroke_width = 1U;
    sc_eye_set_shape(eye_l, 80, 120, 110, 38, 30);
    sc_obj_add_image_texture(eye_l, &k_iris_texture, &k_iris_image);

    sc_obj_t *eye_r = sc_eye_create(screen, &k_eye_r);
    k_eye_r.color = C_WHITE;
    k_eye_r.fill = C_RED;
    k_eye_r.stroke_width = 1U;
    sc_eye_set_shape(eye_r, 240, 120, 110, 38, 30);
    sc_obj_add_image_texture(eye_r, &k_iris_texture, &k_iris_image);

    /* 定时动画：单一相位变量驱动 平移+眨眼（见 sc_eye_demo_timer）。 */
    k_eye_l_obj = eye_l;
    k_eye_r_obj = eye_r;
    s_anim = 0U;
    k_iris_texture.offset_x = IRIS_BASE_OFF_X;
    k_iris_texture.offset_y = IRIS_BASE_OFF_Y;
    sc_add_delay(sc_eye_demo_timer, BLINK_STEP_MS, k_eye_l_obj);
    sc_obj_t *hint = sc_label_create(screen, &k_hint);
    sc_obj_set_text(hint, "iris pans L/R when open, then blinks", SC_FONT_DEFAULT);
    sc_obj_set_pos(hint, 12, SC_SCREEN_HEIGHT - 28);
    sc_invalidate_all();
}

#endif /* SC_USER_OBJ && SC_WIDGET_EYE_ENABLE */
