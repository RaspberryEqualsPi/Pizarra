####################################################################################################
# TGUI - Texus' Graphical User Interface
# Copyright (C) 2012-2025 Bruno Van de Velde (vdv_b@tgui.eu)
#
# This software is provided 'as-is', without any express or implied warranty.
# In no event will the authors be held liable for any damages arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it freely,
# subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented;
#    you must not claim that you wrote the original software.
#    If you use this software in a product, an acknowledgment
#    in the product documentation would be appreciated but is not required.
#
# 2. Altered source versions must be plainly marked as such,
#    and must not be misrepresented as being the original software.
#
# 3. This notice may not be removed or altered from any source distribution.
####################################################################################################

# This script provides the TGUI libraries as imported targets
# ===========================================================
#
# Usage
# -----
#
# Examples:
#   find_package(TGUI 1)
#   find_package(TGUI 1 REQUIRED)
#
# If you have both dynamic and static TGUI libraries installed then you need to set the TGUI_STATIC_LIBRARIES variable
# before calling find_package(TGUI ...) in order to select the right library type.
# example:
#   set(TGUI_STATIC_LIBRARIES TRUE)
#   find_package(TGUI 1 REQUIRED)
#
# On macOS by default CMake will search for frameworks. If you want to use static libraries and have installed
# both TGUI frameworks and TGUI static libraries, your must set CMAKE_FIND_FRAMEWORK to "NEVER" or "LAST"
# in addition to setting TGUI_STATIC_LIBRARIES to TRUE. Otherwise CMake will check the frameworks bundle config and
# fail after finding out that it does not provide static libraries. Please refer to CMake documentation for more details.
#
# Additionally, keep in mind that TGUI frameworks on macOS are only available as release libraries unlike dylibs which
# are available for both release and debug modes.
#
# If TGUI is not installed in a standard path, you can use the TGUI_DIR CMake variable
# to tell CMake where TGUI's config file is located (PREFIX/lib/cmake/TGUI for a library-based installation,
# and PREFIX/TGUI.framework/Resources/CMake on macOS for a framework-based installation).
#
# Output
# ------
#
# This script defines the following variable:
# - TGUI_FOUND:        true if TGUI and all its dependencies are found
#
# And the following target:
#   - TGUI::TGUI
#
# The TGUI target is the same for both Debug and Release build configurations and will automatically provide
# correct settings based on your currently active build configuration. The TGUI target name also does not change
# when using dynamic or static TGUI libraries.
#
# example:
#   find_package(TGUI 1 REQUIRED)
#   add_executable(myapp ...)
#   target_link_libraries(myapp TGUI::TGUI)

include(CMakeFindDependencyMacro)

if(NOT DEFINED TGUI_STATIC_LIBRARIES)
    # If TGUI_STATIC_LIBRARIES is undefined then fall back to TGUI_SHARED_LIBS
    if(DEFINED TGUI_SHARED_LIBS)
        if(TGUI_SHARED_LIBS)
            set(TGUI_STATIC_LIBRARIES FALSE)
        else()
            set(TGUI_STATIC_LIBRARIES TRUE)
        endif()
    else()
        # If TGUI_SHARED_LIBS is also undefined then fall back to whatever the value was when TGUI was build.
        # If libraries were only build for a single configuration then this will be the correct value.
        if(FALSE)
            set(TGUI_STATIC_LIBRARIES FALSE)
        else()
            set(TGUI_STATIC_LIBRARIES TRUE)
        endif()
    endif()
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")

# Search for SFML when a backend uses it
if(NOT DEFINED SFML_STATIC_LIBRARIES)
    if (ON)
        set(SFML_STATIC_LIBRARIES ON)
    elseif (NOT ON)
        set(SFML_STATIC_LIBRARIES ON)
    else() # SFML_STATIC_LIBRARIES wasn't defined when building TGUI
        set(SFML_STATIC_LIBRARIES ${TGUI_STATIC_LIBRARIES})
    endif()
endif()
if ("3.0.0" VERSION_GREATER_EQUAL 3)
    if(NOT TARGET SFML::Graphics AND (ON OR ON))
        find_dependency(SFML 3 CONFIG COMPONENTS Graphics)
    elseif(NOT TARGET SFML::Window AND ON)
        find_dependency(SFML 3 CONFIG COMPONENTS Window)
    endif()
else()
    if(NOT TARGET sfml-graphics AND (ON OR ON))
        find_dependency(SFML 2 CONFIG COMPONENTS graphics)
    elseif(NOT TARGET sfml-window AND ON)
        find_dependency(SFML 2 CONFIG COMPONENTS window)
    endif()
endif()

# Search for GLFW when a backend requires it
if(NOT TARGET glfw AND OFF)
    if()
        find_dependency(glfw3 CONFIG)
    else()
        find_dependency(glfw3)
    endif()
endif()

# Search for raylib when a backend requires it
if(NOT TARGET raylib AND (OFF OR OFF OR OFF))
    if()
        find_dependency(raylib CONFIG)
    else()
        find_dependency(raylib)
    endif()
endif()

# Search for SDL when a backend requires it
if(OFF OR OFF OR OFF OR OFF)
    if()
        if (NOT TARGET SDL3::SDL3)
            find_dependency(SDL3 CONFIG)
        endif()
    elseif()
        find_dependency(SDL2 CONFIG)
    else()
        find_dependency(SDL2)
    endif()
endif()

# Search for SDL_ttf when a backend requires it
if(OFF)
    if()
        if (NOT TARGET SDL3_ttf::SDL3_ttf)
            find_dependency(SDL3_ttf CONFIG)
        endif()
    elseif()
        find_dependency(SDL2_ttf CONFIG)
    else()
        find_dependency(SDL2_ttf)
    endif()
endif()

# When linking statically, we also need to search for libraries that are linked privately
if (TGUI_STATIC_LIBRARIES)
    if(OFF)
        find_dependency(OpenGL)
    endif()

    if(OFF)
        find_dependency(Freetype)
    endif()

    if()
        find_dependency(Threads)
    endif()

    if( AND )
        find_dependency(X11)
    endif()

    if()
        find_dependency(glad CONFIG)
    endif()
endif()

# Choose whether the target definitions of static or shared libs must be imported
if (TGUI_STATIC_LIBRARIES)
	set(TGUI_IS_FRAMEWORK_INSTALL "")
    if (TGUI_IS_FRAMEWORK_INSTALL)
        message(WARNING "Static frameworks are not supported by TGUI. Clear TGUI_DIR cache entry, \
and either change TGUI_STATIC_LIBRARIES or CMAKE_FIND_FRAMEWORK before calling find_package(TGUI)")
    endif()
    set(config_name "Static")
else()
    set(config_name "Shared")
endif()
set(targets_config_file "${CMAKE_CURRENT_LIST_DIR}/TGUI${config_name}Targets.cmake")

# Generate imported target for TGUI
if (EXISTS "${targets_config_file}")
    # Set TGUI_FOUND to TRUE by default, may be overwritten by the include below
    set(TGUI_FOUND TRUE)
    include("${targets_config_file}")
else()
    set(FIND_TGUI_ERROR "Requested TGUI configuration (${config_name}) was not found")
    set(TGUI_FOUND FALSE)
endif()

if (NOT TGUI_FOUND)
    if(TGUI_FIND_REQUIRED)
        # fatal error
        message(FATAL_ERROR "${FIND_TGUI_ERROR}")
    elseif(NOT TGUI_FIND_QUIETLY)
        # error but continue
        message(STATUS "${FIND_TGUI_ERROR}")
    endif()
endif()

if (TGUI_FOUND)
    # Allow linking to "tgui" target for backwards compatibility (when possible)
    if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.18)
        add_library(tgui ALIAS TGUI::TGUI)
    endif()
endif()

if (TGUI_FOUND AND NOT TGUI_FIND_QUIETLY)
    message(STATUS "Found TGUI 1.9.0 in ${CMAKE_CURRENT_LIST_DIR}")
endif()
