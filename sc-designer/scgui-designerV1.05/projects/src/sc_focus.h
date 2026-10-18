#ifndef SC_FOCUS_H
#define SC_FOCUS_H

#include "sc_common.h"
#include "sc_obj.h"

/* 焦点栈：按键用栈管理层级（ENTER 压栈/BACK 弹栈），触摸只设栈顶。
 * focus_stack[focus_depth] 存 obj_id（1字节），sc_obj_at() 可取指针。
 * 按键输入标志用 gui->flags 的 SC_RT_KEY_INPUT 位（1=按键画红框, 0=触摸不画）。
 * 字段直接放在 sc_gui_t 中，少一层引用。 */
#define SC_FOCUS_STACK_MAX 4

/* 焦点栈初始化（sc_gui_init 调用）：depth=0, stack[0]=NIL, 默认按键输入（画红框）。 */
void sc_focus_init(void);
/* 返回当前栈顶焦点对象（focus_stack[focus_depth]）。 */
sc_obj_t *sc_focus_current(void);
/* 弹栈一层：CMD_BACK 返回上级，退出容器内部回到容器本身。 */
void sc_focus_pop(void);
/* 压栈进入内部模式（容器 ENTER）：depth++ 并记录容器。 */
void sc_focus_push(sc_obj_t *obj);
/* 对象释放时清理焦点栈中所有指向该 id 的引用，栈顶被清时 depth 回退。 */
void sc_focus_clear_obj(uint8_t idx);
/* 触摸设置栈顶（DOWN 时调用）：标记触摸输入（不画红框）+ 可聚焦则设栈顶。 */
void sc_focus_touch_set(sc_obj_t *obj);
/* Move keyboard focus to obj, invalidating both the old and new focused objects. */
void sc_obj_set_key_focus(sc_obj_t *obj);
/* 9 宫格空间导航打分核心：以焦点 rect 为界分 3x3 宫格，按方向打分，lower is better，
 * UINT32_MAX 拒绝。全局控件间导航与复合控件内部导航（如软键盘按键矩阵）共用，算法统一。 */
uint32_t sc_focus_grid_score(const rect_t *focus, int key, const rect_t *cand);
/* 统一空间导航入口：depth>0 在栈顶容器子树内导航（内部模式）；depth==0 在活动根
 * 第一层导航（兄弟 + head 直接子级，孙子辈需 ENTER 进入）。
 * focus 为当前焦点（评分起点）；无候选返回 false 并 sc_focus_pop()
 * （内部模式退回容器，全局模式清焦点回卷）。 */
bool sc_obj_focus_root(sc_obj_t *focus, int key);

#endif /* SC_FOCUS_H */
