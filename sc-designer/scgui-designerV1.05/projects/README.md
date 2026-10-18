# scgui SDL Export

这是由 scgui Designer 导出的 SDL 模拟工程。

## 目录说明

- `main.c`: SDL 入口
- `sc_sdl2.c` / `sc_sdl2.h`: SDL 宿主层
- `generated/`: 设计器生成的资源（移植时拷这个目录即可）
  - `app_ui.c` / `app_ui.h`: 导出的界面
  - `sc_config.h`: 工程配置（屏参/控件开关/容量）
  - `images/`, `fonts/`: 导出的图片与字体资源
- `src/`: scgui 核心源码
- `build.bat`: Windows + MinGW 一键编译
- `build.sh`: Linux / MSYS / Git Bash 编译
- `CMakeLists.txt`: CMake 工程

## Windows（推荐）

1. 准备 MinGW gcc。
2. 准备 SDL2 开发包。
3. 二选一：
   - 把 SDL2 解压到工程根目录下的 `sdl2/`
   - 或设置环境变量 `SCGUI_SDL2_DIR` 指向 SDL2 根目录
4. 双击运行 `build.bat`。
5. 生成结果在 `build/` 目录，例如 `build\scgui.exe`。

示例：

```bat
set SCGUI_SDL2_DIR=C:\path\to\sdl2
build.bat
```

## Linux / Git Bash / MSYS

```bash
export SCGUI_SDL2_DIR=/path/to/sdl2
bash build.sh
```

## CMake

```bash
cmake -S . -B build-cmake -DSCGUI_SDL2_DIR=/path/to/sdl2
cmake --build build-cmake
```

## 运行

- Windows: 运行 `build\scgui.exe`
- 其他平台: 运行 `build/scgui` 或 CMake 产物
