cmake_minimum_required(VERSION 3.25)
project(train_tycoon LANGUAGES C)

if(NOT WIN32)
    message(FATAL_ERROR "Train Tycoon needs Windows.")
endif()

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

file(GLOB GAME_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/*.c
)

add_executable(train_tycoon
    src/main.c
    src/midp_graphics.c
    src/midp_image.c
    src/midp_input.c
    src/midp_media_win32.c
    src/midp_resource.c
    src/midp_rms.c
    src/midp_stub.c
    src/midp_util.c
    src/game_state.c
    src/third_party/stb_image_impl.c
    ${GAME_SOURCES}
)

target_include_directories(train_tycoon PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/third_party
)

target_compile_definitions(train_tycoon PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(train_tycoon PRIVATE -Wall -Wextra)
elseif(MSVC)
    target_compile_options(train_tycoon PRIVATE /std:clatest)
endif()

target_link_libraries(train_tycoon PRIVATE user32 gdi32 winmm)

add_custom_command(TARGET train_tycoon POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/resources
            $<TARGET_FILE_DIR:train_tycoon>/resources
)

set_property(TARGET train_tycoon PROPERTY
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:train_tycoon>"
)

enable_testing()
add_test(NAME train_tycoon_smoke COMMAND $<TARGET_FILE:train_tycoon>)
set_tests_properties(train_tycoon_smoke PROPERTIES
    ENVIRONMENT "TRAIN_TYCOON_SMOKE=120"
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:train_tycoon>"
)
