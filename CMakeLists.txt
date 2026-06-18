cmake_minimum_required(VERSION 3.20)

project(ImGuiAddonsPreview LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# ============================================================
# User-configurable options visible in cmake-gui
# ============================================================

option(IMGADDONS_BUILD_PREVIEW "Build the ImGui-Addons preview executable" ON)

set(IMGADDONS_IMGUI_DIR
    "${CMAKE_SOURCE_DIR}/imgui"
    CACHE PATH
    "Path to Dear ImGui folder containing imgui.h, imgui.cpp, etc."
)

set(IMGADDONS_IMGUI_BACKEND_DIR
    "${CMAKE_SOURCE_DIR}/imgui/backends"
    CACHE PATH
    "Path to Dear ImGui backend folder containing imgui_impl_glfw.cpp and imgui_impl_opengl3.cpp."
)

set(IMGADDONS_FILEBROWSER_DIR
    "${CMAKE_SOURCE_DIR}/FileBrowser"
    CACHE PATH
    "Path to FileBrowser addon folder containing ImGuiFileBrowser.cpp/.h and Dirent/dirent.h."
)

set(IMGADDONS_GLFW_FETCH_DIR
    "${CMAKE_BINARY_DIR}/_deps/glfw"
    CACHE PATH
    "Directory where GLFW will be downloaded and built by FetchContent."
)

set(IMGADDONS_GLFW_GIT_TAG
    "3.4"
    CACHE STRING
    "GLFW git tag, branch, or commit to fetch."
)

set(IMGADDONS_RUNTIME_OUTPUT_DIR
    "${CMAKE_BINARY_DIR}/bin"
    CACHE PATH
    "Directory where the preview executable will be written."
)

# ============================================================
# Validate expected repo layout / user-selected paths
# ============================================================

if(NOT EXISTS "${IMGADDONS_IMGUI_DIR}/imgui.h")
    message(FATAL_ERROR
        "IMGADDONS_IMGUI_DIR does not contain imgui.h:\n"
        "${IMGADDONS_IMGUI_DIR}"
    )
endif()

if(NOT EXISTS "${IMGADDONS_IMGUI_DIR}/imgui.cpp")
    message(FATAL_ERROR
        "IMGADDONS_IMGUI_DIR does not contain imgui.cpp:\n"
        "${IMGADDONS_IMGUI_DIR}"
    )
endif()

if(NOT EXISTS "${IMGADDONS_IMGUI_DIR}/imgui_draw.cpp")
    message(FATAL_ERROR
        "IMGADDONS_IMGUI_DIR does not contain imgui_draw.cpp:\n"
        "${IMGADDONS_IMGUI_DIR}"
    )
endif()

if(NOT EXISTS "${IMGADDONS_IMGUI_BACKEND_DIR}/imgui_impl_glfw.cpp")
    message(FATAL_ERROR
        "IMGADDONS_IMGUI_BACKEND_DIR does not contain imgui_impl_glfw.cpp:\n"
        "${IMGADDONS_IMGUI_BACKEND_DIR}"
    )
endif()

if(NOT EXISTS "${IMGADDONS_IMGUI_BACKEND_DIR}/imgui_impl_opengl3.cpp")
    message(FATAL_ERROR
        "IMGADDONS_IMGUI_BACKEND_DIR does not contain imgui_impl_opengl3.cpp:\n"
        "${IMGADDONS_IMGUI_BACKEND_DIR}"
    )
endif()

if(NOT EXISTS "${IMGADDONS_FILEBROWSER_DIR}/ImGuiFileBrowser.h")
    message(FATAL_ERROR
        "IMGADDONS_FILEBROWSER_DIR does not contain ImGuiFileBrowser.h:\n"
        "${IMGADDONS_FILEBROWSER_DIR}"
    )
endif()

if(NOT EXISTS "${IMGADDONS_FILEBROWSER_DIR}/ImGuiFileBrowser.cpp")
    message(FATAL_ERROR
        "IMGADDONS_FILEBROWSER_DIR does not contain ImGuiFileBrowser.cpp:\n"
        "${IMGADDONS_FILEBROWSER_DIR}"
    )
endif()

if(NOT EXISTS "${IMGADDONS_FILEBROWSER_DIR}/Dirent/dirent.h")
    message(FATAL_ERROR
        "Dirent/dirent.h was not found here:\n"
        "${IMGADDONS_FILEBROWSER_DIR}/Dirent/dirent.h"
    )
endif()

message(STATUS "Using ImGui from: ${IMGADDONS_IMGUI_DIR}")
message(STATUS "Using ImGui backends from: ${IMGADDONS_IMGUI_BACKEND_DIR}")
message(STATUS "Using FileBrowser addon from: ${IMGADDONS_FILEBROWSER_DIR}")
message(STATUS "GLFW fetch/build directory: ${IMGADDONS_GLFW_FETCH_DIR}")
message(STATUS "Preview executable output directory: ${IMGADDONS_RUNTIME_OUTPUT_DIR}")

# ============================================================
# ImGui source files
# Supports older and newer ImGui versions.
# ============================================================

set(IMGADDONS_IMGUI_SOURCES
    "${IMGADDONS_IMGUI_DIR}/imgui.cpp"
    "${IMGADDONS_IMGUI_DIR}/imgui_draw.cpp"
    "${IMGADDONS_IMGUI_DIR}/imgui_demo.cpp"
)

if(EXISTS "${IMGADDONS_IMGUI_DIR}/imgui_widgets.cpp")
    list(APPEND IMGADDONS_IMGUI_SOURCES
        "${IMGADDONS_IMGUI_DIR}/imgui_widgets.cpp"
    )
endif()

if(EXISTS "${IMGADDONS_IMGUI_DIR}/imgui_tables.cpp")
    list(APPEND IMGADDONS_IMGUI_SOURCES
        "${IMGADDONS_IMGUI_DIR}/imgui_tables.cpp"
    )
endif()

set(IMGADDONS_IMGUI_BACKEND_SOURCES
    "${IMGADDONS_IMGUI_BACKEND_DIR}/imgui_impl_glfw.cpp"
    "${IMGADDONS_IMGUI_BACKEND_DIR}/imgui_impl_opengl3.cpp"
)

set(IMGADDONS_ADDON_SOURCES
    "${IMGADDONS_FILEBROWSER_DIR}/ImGuiFileBrowser.cpp"
)

# ============================================================
# Fetch GLFW
# ============================================================

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        ${IMGADDONS_GLFW_GIT_TAG}
    GIT_SHALLOW    TRUE
    SOURCE_DIR     "${IMGADDONS_GLFW_FETCH_DIR}/src"
    BINARY_DIR     "${IMGADDONS_GLFW_FETCH_DIR}/build"
)

FetchContent_MakeAvailable(glfw)

# ============================================================
# OpenGL
# ============================================================

find_package(OpenGL REQUIRED)

# CMake's FindOpenGL provides imported targets like OpenGL::GL
# for the platform-specific OpenGL library.
set(IMGADDONS_PLATFORM_LIBS OpenGL::GL)

if(WIN32)
    # Usually GLFW handles most Windows system libs itself.
    # imm32 is commonly needed by ImGui's Win32 input/IME path in some setups.
    list(APPEND IMGADDONS_PLATFORM_LIBS imm32)
endif()

# ============================================================
# Preview executable
# You will provide preview/main.cpp yourself.
# ============================================================

if(IMGADDONS_BUILD_PREVIEW)

    add_executable(ImGuiAddonsPreview
        "preview/main.cpp"

        ${IMGADDONS_ADDON_SOURCES}
        ${IMGADDONS_IMGUI_SOURCES}
        ${IMGADDONS_IMGUI_BACKEND_SOURCES}
    )

    target_include_directories(ImGuiAddonsPreview PRIVATE
        "${IMGADDONS_IMGUI_DIR}"
        "${IMGADDONS_IMGUI_BACKEND_DIR}"
        "${IMGADDONS_FILEBROWSER_DIR}"
    )

    target_link_libraries(ImGuiAddonsPreview PRIVATE
        glfw
        ${IMGADDONS_PLATFORM_LIBS}
    )

    # MSVC niceties; does not affect MinGW.
    if(MSVC)
        target_compile_options(ImGuiAddonsPreview PRIVATE
            /permissive-
            /W4
        )
    else()
        target_compile_options(ImGuiAddonsPreview PRIVATE
            -Wall
            -Wextra
        )
    endif()

    set_target_properties(ImGuiAddonsPreview PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${IMGADDONS_RUNTIME_OUTPUT_DIR}"
    )

    foreach(CONFIG_TYPE Debug Release RelWithDebInfo MinSizeRel)
        string(TOUPPER "${CONFIG_TYPE}" CONFIG_TYPE_UPPER)
        set_target_properties(ImGuiAddonsPreview PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE_UPPER}
            "${IMGADDONS_RUNTIME_OUTPUT_DIR}"
        )
    endforeach()

endif()