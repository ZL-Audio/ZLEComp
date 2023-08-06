# 3.24.1 is bundled in Visual Studio 2022 v17.4
# 3.24.1 is also bundled in CLion as of 2023
cmake_minimum_required(VERSION 3.24.1)

# This is the name of your plugin
# This cannot have spaces (but PRODUCT_NAME can)
set(PROJECT_NAME "ZLEComp")

# Set the plugin formats you'll be building here.
# Valid formats: AAX Unity VST AU AUv3 Standalone
set(FORMATS AU VST3)

# This must be set before the project() call
# see: https://cmake.org/cmake/help/latest/variable/CMAKE_OSX_DEPLOYMENT_TARGET.html
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Support macOS down to High Sierra")

# Reads in our VERSION file and sticks in it CURRENT_VERSION variable
# Be sure the file has no newlines!
file(STRINGS VERSION CURRENT_VERSION)

# For simplicity, the name of the project is also the name of the target
project(${PROJECT_NAME} VERSION ${CURRENT_VERSION})

# By default we don't want Xcode schemes to be made for modules, etc
set(CMAKE_XCODE_GENERATE_SCHEME OFF)

find_package(Boost 1.80.0 REQUIRED)

# Building universal binaries on macOS increases build time
# This is set on CI but not during local dev
if ((DEFINED ENV{CI}) AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
    message("Building for Apple Silicon and x86_64")
    set(CMAKE_OSX_ARCHITECTURES arm64 x86_64)
endif ()

# Adds all the module sources so they appear correctly in the IDE
# Must be set before JUCE is added as a sub-dir (or any targets are made)
# https://github.com/juce-framework/JUCE/commit/6b1b4cf7f6b1008db44411f2c8887d71a3348889
set_property(GLOBAL PROPERTY USE_FOLDERS YES)

# Create a /Modules directory in the IDE with the JUCE Module code
option(JUCE_ENABLE_MODULE_SOURCE_GROUPS "Show all module sources in IDE projects" ON)

# JUCE is setup as a submodule in the /JUCE folder
# Locally, you'll need to run `git submodule update --init --recursive` once
# and `git submodule update --remote --merge` to keep it up to date
# On Github Actions, it's managed by actions/checkout
add_subdirectory(JUCE)

# Check the readme at `docs/CMake API.md` in the JUCE repo for full config
juce_add_plugin("${PROJECT_NAME}"
        # VERSION ...                               # Set this if the plugin version is different to the project version
        # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
        # ICON_SMALL ...
        COMPANY_NAME ZL
        IS_SYNTH FALSE                       # Is this a synth or an effect?
        NEEDS_MIDI_INPUT FALSE               # Does the plugin need midi input?
        NEEDS_MIDI_OUTPUT FALSE              # Does the plugin need midi output?
        IS_MIDI_EFFECT FALSE                 # Is this plugin a MIDI effect?
        EDITOR_WANTS_KEYBOARD_FOCUS TRUE    # Does the editor need keyboard focus?
        COPY_PLUGIN_AFTER_BUILD TRUE                # On MacOS, plugin will be copied to /Users/you/Library/Audio/Plug-Ins/
        PLUGIN_MANUFACTURER_CODE Zliu               # This has to be one uppercase, rest lower for AU formats
        PLUGIN_CODE Ecom                            # A unique four-character plugin id with at least one upper-case character
        VST3_CATEGORIES Dynamics
        AU_MAIN_TYPE kAudioUnitType_Effect
        FORMATS "${FORMATS}"
        PRODUCT_NAME "ZL E Compressor")             # The name of the final executable, which can differ from the target name

# C++20, please
target_compile_features("${PROJECT_NAME}" PRIVATE cxx_std_20)

# Manually list all .h and .cpp files for the plugin
set(SourceFiles
        Source/PluginEditor.h
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginProcessor.cpp

        Source/State/state_definitions.h

        Source/DSP/dsp_definitions.h
        Source/DSP/controller.h
        Source/DSP/controller.cpp
        Source/DSP/controller_attach.h
        Source/DSP/controller_attach.cpp
        Source/DSP/detector_attach.h
        Source/DSP/detector_attach.cpp
        Source/DSP/computer_attach.h
        Source/DSP/computer_attach.cpp
        Source/DSP/FixedBuffer/fifo_audio_buffer.h
        Source/DSP/FixedBuffer/fifo_audio_buffer.cpp
        Source/DSP/FixedBuffer/fixed_audio_buffer.h
        Source/DSP/FixedBuffer/fixed_audio_buffer.cpp
        Source/DSP/Computer/computer.h
        Source/DSP/Computer/computer.cpp
        Source/DSP/Detector/iter_funcs.h
        Source/DSP/Detector/detector.h
        Source/DSP/Detector/detector.cpp
        Source/DSP/Detector/tracker.h
        Source/DSP/Detector/rms_tracker.h
        Source/DSP/Detector/rms_tracker.cpp

        Source/GUI/interface_definitions.h
        Source/GUI/rotary_slider_look_and_feel.h
        Source/GUI/rotary_slider_component.h
        Source/GUI/button_look_and_feel.h
        Source/GUI/button_component.h
        Source/GUI/combobox_look_and_feel.h
        Source/GUI/combo_box_component.h
        Source/GUI/name_look_and_feel.h

        Source/Panel/main_panel.h
        Source/Panel/main_panel.cpp
        Source/Panel/logo_panel.h
        Source/Panel/logo_panel.cpp
        Source/Panel/plot_panel.h
        Source/Panel/plot_panel.cpp
        )
target_sources("${PROJECT_NAME}" PRIVATE ${SourceFiles})

# No, we don't want our source buried in extra nested folders
set_target_properties("${PROJECT_NAME}" PROPERTIES FOLDER "")

# The Xcode source tree should uhhh, still look like the source tree, yo
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Source PREFIX "" FILES ${SourceFiles})

# Setup our binary data as a target
juce_add_binary_data(Assets SOURCES
        resources/zlaudio.svg
        resources/logo.svg
        resources/font/OpenSans-SemiBold.ttf

        resources/presets/nothing.xml
        resources/presets/default.xml
        resources/presets/half_rms_m.xml)

# Required for Linux happiness:
# See https://forum.juce.com/t/loading-pytorch-model-using-binarydata/39997/2
set_target_properties(Assets PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

# Clean up folder organization on Xcode.
# It tucks the Plugin varieties into a "Targets" folder and generate an Xcode Scheme manually
# Xcode scheme generation is turned off globally to limit noise from other targets
# The non-hacky way of doing this is via the global PREDEFINED_TARGETS_FOLDER property
# However that doesn't seem to be working in Xcode
# Not all plugin types (au, vst) available on each build type (win, macos, linux)
foreach (target ${FORMATS} "All")
    if (TARGET ${PROJECT_NAME}_${target})
        set_target_properties(${PROJECT_NAME}_${target} PROPERTIES
                # Tuck the actual plugin targets into a folder where they won't bother us
                FOLDER "Targets"

                # MacOS only: Sets the default executable that Xcode will open on build
                # For this exact path to to work, manually build the AudioPluginHost.xcodeproj in the JUCE subdir
                XCODE_SCHEME_EXECUTABLE "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/extras/AudioPluginHost/Builds/MacOSX/build/Debug/AudioPluginHost.app"

                # Let us build the target in Xcode
                XCODE_GENERATE_SCHEME ON)
    endif ()
endforeach ()
set_target_properties(Assets PROPERTIES FOLDER "Targets")


target_compile_definitions("${PROJECT_NAME}"
        PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0)

target_link_libraries("${PROJECT_NAME}"
        PRIVATE
        Assets
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_audio_processors
        Boost::boost
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# When present, use Intel IPP for performance on Windows
if (WIN32) # Can't use MSVC here, as it won't catch Clang on Windows
    find_package(IPP)
    if (IPP_FOUND)
        target_link_libraries("${PROJECT_NAME}" PUBLIC IPP::ipps IPP::ippcore IPP::ippi IPP::ippcv)
        message("IPP LIBRARIES FOUND")
        target_compile_definitions("${PROJECT_NAME}" PUBLIC PAMPLEJUCE_IPP=1)
    else ()
        message("IPP LIBRARIES *NOT* FOUND")
    endif ()
endif ()