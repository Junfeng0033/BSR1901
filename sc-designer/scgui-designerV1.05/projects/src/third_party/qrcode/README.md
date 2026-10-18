# QRCode（面向小 MCU 的裁剪移植）

上游：[ricmoo/QRCode](https://github.com/ricmoo/QRCode)，作者 Richard Moore；其实现部分派生自 Project Nayuki 的 QR Code generator。许可证为 MIT，详见 `LICENSE`，版权和许可声明也完整保留在 `qrcode.c`/`qrcode.h` 中。

## 选型

| 实现 | 许可证 | 内存方式 | MCU 取舍 |
| --- | --- | --- | --- |
| ricmoo/QRCode | MIT | 调用方缓冲区 + 栈，无堆 | 有固定 Version 路径，易于按容量裁剪；本项目采用 |
| Nayuki QR-Code-generator C | MIT | 调用方提供临时区和输出区，无堆 | 完整支持 Version 1–40，通用能力强，但会保留更多通用路径 |
| libqrencode | LGPL-2.1 | 默认动态分配 | 功能完整，依赖和许可证约束更重，不适合 4KB RAM 的最小固件 |

## 编译期容量配置

在 `src/third_party/qrcode/qrcode.h` 中配置 `SC_QCODE_MAX_BYTES`，也可通过编译参数覆盖：

```c
#define SC_QCODE_MAX_BYTES 17U /* 可选：17U、32U、53U */
```

```text
-DSC_QCODE_MAX_BYTES=32
```

每次构建只保留选中版本，不是在运行时同时携带三个编码器。

| `SC_QCODE_MAX_BYTES` | QR 版本 | 矩阵 | 含 4 模块静区的最小边长 | 模块缓冲区 | `sizeof(sc_qcode_t)` |
| ---: | ---: | ---: | ---: | ---: | ---: |
| 17 | Version 1-L | 21×21 | 29 px | 56 B | 62 B |
| 32 | Version 2-L | 25×25 | 33 px | 79 B | 86 B |
| 53 | Version 3-L | 29×29 | 37 px | 106 B | 112 B |

只允许这三个值，其他值会在编译期报错。容量按字节计算，不是按 UTF-8 字符数计算。

## 固定裁剪特性

- L 级纠错；
- Byte mode；
- 不使用堆；
- 编码临时数据仅在调用期间使用栈；
- 无 VLA，兼容不实现可选 C11 VLA 的 MCU 编译器；
- 通过预处理移除模式探测、其他版本表和未使用公共 API。

精简后的直接 API 为：

```c
int8_t qrcode_initText(QRCode *qrcode, uint8_t *modules, const char *data);
```

成功返回 `0`；空指针或超过当前 `SC_QCODE_MAX_BYTES` 时返回 `-1`。`modules` 必须至少提供 `QRCODE_BUFFER_SIZE` 字节。应用通常应使用 `src/widget/sc_qcode.h` 中的 `sc_qcode_create()` 和 `sc_qcode_set_text()`。

## 测量

MinGW GCC `-Os` 的编码器对象测量：

| 容量 | `text` | `data` | `bss` | `qrcode_initText` 主函数栈 | 含嵌套调用的保守栈估算 |
| ---: | ---: | ---: | ---: | ---: | ---: |
| 17 B | 3516 B | 0 | 0 | 336 B | 约 640 B |
| 32 B | 3820 B | 0 | 0 | 416 B | 约 720 B |
| 53 B | 3820 B | 0 | 0 | 496 B | 约 800 B |

当前完整 PC SDL（`build_demo`）链接测量：

| 配置 | `text` | `data` | `bss` |
| --- | ---: | ---: | ---: |
| qcode 关闭 | 58680 | 164752 | 2416 |
| 17 B | 62956 | 164816 | 2416 |
| 32 B | 63260 | 164848 | 2416 |
| 53 B | 63260 | 164880 | 2416 |

主机 ABI 与 MCU ABI 差异较大，最终 Flash 和栈占用应以目标 MCU 工具链结果为准。通过 `SC_WIDGET_QCODE_ENABLE=0` 可从固件中裁掉控件和编码器。
