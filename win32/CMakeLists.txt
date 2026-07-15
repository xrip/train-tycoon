cmake_minimum_required(VERSION 3.25)
project(train_tycoon LANGUAGES C CXX)

if(NOT WIN32)
    message(FATAL_ERROR "Train Tycoon needs Windows.")
endif()

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB GAME_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/*.c
)

add_library(train_tycoon_legacy STATIC
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

target_include_directories(train_tycoon_legacy PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/third_party
)

target_compile_definitions(train_tycoon_legacy PUBLIC WIN32_LEAN_AND_MEAN NOMINMAX)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(train_tycoon_legacy PRIVATE -Wall -Wextra)
elseif(MSVC)
    target_compile_options(train_tycoon_legacy PRIVATE /std:clatest)
endif()

add_executable(train_tycoon src/main.c)
target_link_libraries(train_tycoon PRIVATE
    train_tycoon_legacy
    user32
    gdi32
    winmm
)

add_custom_command(TARGET train_tycoon POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/resources
            $<TARGET_FILE_DIR:train_tycoon>/resources
)

set_property(TARGET train_tycoon PROPERTY
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:train_tycoon>"
)

enable_testing()
add_executable(train_tycoon_tests
    tests/test_main.cpp
    tests/test_smoke.cpp
)
target_link_libraries(train_tycoon_tests PRIVATE train_tycoon_legacy winmm)

set(TEST_RUN_DIR "$<TARGET_FILE_DIR:train_tycoon_tests>/test_run")
add_custom_command(TARGET train_tycoon_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${TEST_RUN_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/resources
            ${TEST_RUN_DIR}/resources
)

add_test(NAME train_tycoon_fast
    COMMAND $<TARGET_FILE:train_tycoon_tests> "--test-case=fast:*")
set_tests_properties(train_tycoon_fast PROPERTIES
    WORKING_DIRECTORY "${TEST_RUN_DIR}"
    LABELS fast
)

add_test(NAME train_tycoon_full
    COMMAND $<TARGET_FILE:train_tycoon_tests> "--test-case=full:*")
set_tests_properties(train_tycoon_full PROPERTIES
    WORKING_DIRECTORY "${TEST_RUN_DIR}"
    LABELS full
)

option(TRAIN_TYCOON_ENABLE_DEVICE_TESTS "Add tests that need local sound hardware" OFF)
if(TRAIN_TYCOON_ENABLE_DEVICE_TESTS)
    add_test(NAME train_tycoon_midi_device COMMAND $<TARGET_FILE:train_tycoon>)
    set_tests_properties(train_tycoon_midi_device PROPERTIES
        WORKING_DIRECTORY "$<TARGET_FILE_DIR:train_tycoon>"
        ENVIRONMENT "TRAIN_TYCOON_MIDI_TEST_MS=1000"
        LABELS device
    )
endif()
