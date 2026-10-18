#ifndef SC_MEM_H
#define SC_MEM_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 轻量内存/字符串工具，替代标准库 <string.h>，供无 libc 的 MCU 环境使用。
 * 签名与标准库对齐，返回 dst 原指针，便于链式调用。 */

void  *sc_memset(void *dst, int val, size_t n);          /* 填充 n 字节，字对齐优化 */
void  *sc_memmove(void *dst, const void *src, size_t n); /* 重叠安全（正/倒序拷贝） */
void  *sc_memcpy(void *dst, const void *src, size_t n);  /* 拷贝 n 字节，字对齐优化 */
size_t sc_strlen(const char *s);                         /* 返回字符串长度（不含 '\0'） */

#ifdef __cplusplus
}
#endif

#endif /* SC_MEM_H */
