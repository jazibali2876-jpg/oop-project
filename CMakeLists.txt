cmake_minimum_required(VERSION 3.20)
project(SmartPOS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/dist")

# --- SFML (rebuilt with MinGW under third_party/SFML-mingw) ---------------
set(SFML_DIR "${CMAKE_SOURCE_DIR}/third_party/SFML-mingw/lib/cmake/SFML")
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System Audio CONFIG)

# --- ImGui (vendored, compiled as part of our target) ---------------------
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/imgui-master")
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# --- Project sources ------------------------------------------------------
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.h"
)

add_executable(smart_pos
    ${PROJECT_SOURCES}
    ${IMGUI_SOURCES}
)

target_include_directories(smart_pos PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(smart_pos PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
    opengl32
)

if(WIN32)
    target_compile_definitions(smart_pos PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

# --- Post-build: copy SFML + MinGW runtime DLLs next to the .exe ----------
set(SFML_BIN_DIR "${CMAKE_SOURCE_DIR}/third_party/SFML-mingw/bin")
set(MINGW_BIN_DIR "${CMAKE_SOURCE_DIR}/mingw64/bin")

set(RUNTIME_DLLS
    "${SFML_BIN_DIR}/sfml-graphics-3.dll"
    "${SFML_BIN_DIR}/sfml-window-3.dll"
    "${SFML_BIN_DIR}/sfml-system-3.dll"
    "${SFML_BIN_DIR}/sfml-audio-3.dll"
    "${MINGW_BIN_DIR}/libgcc_s_seh-1.dll"
    "${MINGW_BIN_DIR}/libstdc++-6.dll"
    "${MINGW_BIN_DIR}/libwinpthread-1.dll"
)

foreach(_dll ${RUNTIME_DLLS})
    if(EXISTS "${_dll}")
        add_custom_command(TARGET smart_pos POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${_dll}" "$<TARGET_FILE_DIR:smart_pos>")
    endif()
endforeach()

# --- Post-build: copy assets/ next to the .exe; ensure data/ folder exists -
add_custom_command(TARGET smart_pos POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:smart_pos>/assets"
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:smart_pos>/data"
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:smart_pos>/data/receipts")
