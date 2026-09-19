cmake_minimum_required(VERSION 3.20)

# Keep these versions aligned with the Raspberry Pi Pico VS Code extension.
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.3.0)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if(EXISTS ${picoVscode})
    include(${picoVscode})
endif()

set(PICO_BOARD seeed_xiao_rp2350 CACHE STRING "Board type")

# RP2350 uses TinyUSB 0.21.0 fetched by this project, not pico-sdk's copy.
include(FetchContent)

function(apply_dependency_overrides SOURCE_DIR OVERRIDE_DIR)
    if(NOT EXISTS "${OVERRIDE_DIR}")
        return()
    endif()

    file(GLOB_RECURSE override_files
        RELATIVE "${OVERRIDE_DIR}"
        CONFIGURE_DEPENDS
        "${OVERRIDE_DIR}/*")
    foreach(relative_path IN LISTS override_files)
        if(NOT IS_DIRECTORY "${OVERRIDE_DIR}/${relative_path}")
            get_filename_component(destination_dir
                "${SOURCE_DIR}/${relative_path}" DIRECTORY)
            file(MAKE_DIRECTORY "${destination_dir}")
            configure_file(
                "${OVERRIDE_DIR}/${relative_path}"
                "${SOURCE_DIR}/${relative_path}"
                COPYONLY)
        endif()
    endforeach()
endfunction()

FetchContent_Declare(tinyusb_021
    GIT_REPOSITORY https://github.com/hathach/tinyusb.git
    GIT_TAG dae3f9a366bfcddbf9dcf1b48d7500286a849539
    GIT_SHALLOW TRUE
    GIT_SUBMODULES ""
    UPDATE_DISCONNECTED TRUE
    SOURCE_SUBDIR cmake-not-used)
FetchContent_MakeAvailable(tinyusb_021)
apply_dependency_overrides(
    "${tinyusb_021_SOURCE_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/tinyusb-overrides")
set(PICO_TINYUSB_PATH "${tinyusb_021_SOURCE_DIR}" CACHE PATH "TinyUSB 0.21.0 source" FORCE)

include(pico_sdk_import.cmake)

project(MSXPAD2USB C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

pico_sdk_init()

add_executable(MSXPAD2USB
    src/main.c
    src/usb_descriptors.c)

target_include_directories(MSXPAD2USB PRIVATE src)
target_compile_options(MSXPAD2USB PRIVATE -Wall -Wextra -Werror)
target_link_options(MSXPAD2USB PRIVATE -Xlinker --print-memory-usage)

target_link_libraries(MSXPAD2USB PRIVATE
    pico_stdlib
    tinyusb_device)

pico_enable_stdio_uart(MSXPAD2USB 0)
pico_enable_stdio_usb(MSXPAD2USB 0)
pico_add_extra_outputs(MSXPAD2USB)

add_custom_command(TARGET MSXPAD2USB POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_LIST_DIR}/uf2"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE_DIR:MSXPAD2USB>/MSXPAD2USB.uf2"
        "${CMAKE_CURRENT_LIST_DIR}/uf2/MSXPAD2USB.uf2"
    COMMENT "Copying MSXPAD2USB.uf2 to the uf2 directory")
