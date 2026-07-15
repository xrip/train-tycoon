cmake_minimum_required(VERSION 3.25)
project(train_tycoon LANGUAGES C)

# Native Win32 port of the Train Tycoon J2ME game on the MiniFB backend.
# Sources split into three layers:
#   port/           - hand-written MIDP shim + Win32 entry (owned, stable)
#   port/generated/ - tools/port.py output: struct, init, prototypes, stubs
#   port/impl/      - hand-ported GameCanvas method bodies (owned, the game)
if(NOT WIN32)
    message(FATAL_ERROR "This port uses the Win32 MiniFB backend and must be built for Windows.")
endif()

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Hand-ported method bodies grow over time; glob so new files need no edit here.
file(GLOB PORT_IMPL CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/port/impl/*.c)

add_executable(train_tycoon
    port/main.c
    port/midp_graphics.c
    port/midp_image.c
    port/midp_resource.c
    port/midp_rms.c
    port/midp_util.c
    port/midp_input.c
    port/midp_media_win32.c
    port/midp_stub.c
    port/generated/game_state.c
    port/generated/game_stubs.c
    port/third_party/stb_image_impl.c
    ${PORT_IMPL}
)

target_include_directories(train_tycoon PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}            # MiniFB.h at repo root
    ${CMAKE_CURRENT_SOURCE_DIR}/port
    ${CMAKE_CURRENT_SOURCE_DIR}/port/generated
    ${CMAKE_CURRENT_SOURCE_DIR}/port/third_party
)

target_compile_definitions(train_tycoon PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(train_tycoon PRIVATE -Wall -Wextra)
elseif(MSVC)
    # CMAKE_C_STANDARD 23 does not reliably select C23 on the VS generator.
    target_compile_options(train_tycoon PRIVATE /std:clatest)
endif()

target_link_libraries(train_tycoon PRIVATE user32 gdi32 winmm)
