cmake_minimum_required(VERSION 3.22)

project(ZainsDemon VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(JUCE_DIR "" CACHE PATH "Path to JUCE")

if(NOT JUCE_DIR)
    message(FATAL_ERROR "Set JUCE_DIR to your JUCE folder.")
endif()

# JUCE'yi projeye dahil et
add_subdirectory(${JUCE_DIR} JUCE)

# Plugin oluştur
juce_add_plugin(ZainsDemon
    COMPANY_NAME "Zain"
    PRODUCT_NAME "Zains Demon"

    PLUGIN_MANUFACTURER_CODE Zain
    PLUGIN_CODE ZDem

    FORMATS VST3 Standalone

    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE

    COPY_PLUGIN_AFTER_BUILD TRUE
)

# JuceHeader.h oluştur
juce_generate_juce_header(ZainsDemon)

# Kaynak dosyaları
target_sources(ZainsDemon PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/PitchShifter.h
)

# JUCE ayarları
target_compile_definitions(ZainsDemon PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

# JUCE kütüphaneleri
target_link_libraries(ZainsDemon PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
)

# Plugin'in JUCE modüllerine erişebilmesi için gerekli
target_include_directories(ZainsDemon PRIVATE
    ${JUCE_DIR}/modules
)
