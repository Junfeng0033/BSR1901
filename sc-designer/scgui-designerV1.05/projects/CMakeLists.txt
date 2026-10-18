cmake_minimum_required(VERSION 3.16)
project(scgui_sdl C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Optional. Prefer build.bat / build.sh when CMake is not installed.
# ---- Edit this path to your SDL2 SDK root (must contain include/ and lib/) ----
set(SCGUI_SDL2_DIR "${CMAKE_SOURCE_DIR}/sdl2" CACHE PATH "SDL2 SDK root")

if(NOT EXISTS "${SCGUI_SDL2_DIR}/include/SDL2/SDL.h")
  message(FATAL_ERROR "SDL2 headers not found. Set -DSCGUI_SDL2_DIR=... to your SDL2 root.")
endif()

file(GLOB SCGUI_CORE_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_SOURCE_DIR}/src/*.c"
  "${CMAKE_SOURCE_DIR}/src/widget/*.c"
  "${CMAKE_SOURCE_DIR}/src/third_party/qrcode/*.c"
)
file(GLOB SCGUI_GENERATED_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/generated/*.c")
file(GLOB SCGUI_IMAGE_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/generated/images/*.c")
file(GLOB SCGUI_FONT_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/generated/fonts/*.c")

add_executable(scgui
  main.c
  sc_sdl2.c
  ${SCGUI_CORE_SOURCES}
  ${SCGUI_GENERATED_SOURCES}
  ${SCGUI_IMAGE_SOURCES}
  ${SCGUI_FONT_SOURCES}
)

target_include_directories(scgui PRIVATE
  "${CMAKE_SOURCE_DIR}"
  "${CMAKE_SOURCE_DIR}/src"
  "${CMAKE_SOURCE_DIR}/generated"
  "${CMAKE_SOURCE_DIR}/generated/images"
  "${CMAKE_SOURCE_DIR}/generated/fonts"
  "${SCGUI_SDL2_DIR}/include"
)

if(MSVC)
  target_link_directories(scgui PRIVATE "${SCGUI_SDL2_DIR}/lib")
  target_link_libraries(scgui PRIVATE SDL2 SDL2main)
else()
  target_link_directories(scgui PRIVATE "${SCGUI_SDL2_DIR}/lib")
  target_link_libraries(scgui PRIVATE mingw32 SDL2main SDL2)
  if(WIN32)
    target_link_options(scgui PRIVATE -Wl,--gc-sections)
  endif()
endif()

if(EXISTS "${SCGUI_SDL2_DIR}/bin/SDL2.dll")
  add_custom_command(TARGET scgui POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SCGUI_SDL2_DIR}/bin/SDL2.dll"
            "$<TARGET_FILE_DIR:scgui>/SDL2.dll")
endif()
