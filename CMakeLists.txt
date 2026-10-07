cmake_minimum_required(VERSION 3.22)
project(NoteShaper VERSION 0.5.1 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT DEFINED JUCE_PATH)
  message(FATAL_ERROR "Set JUCE_PATH to the JUCE 7.0.12 source directory")
endif()
add_subdirectory("${JUCE_PATH}" JUCE)
juce_add_plugin(NoteShaper
  COMPANY_NAME "NoteShaper"
  BUNDLE_ID "local.notefollow.tuner"
  PLUGIN_MANUFACTURER_CODE Ntfw
  PLUGIN_CODE Nf01
  FORMATS VST3 Standalone
  PRODUCT_NAME "NoteShaper"
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT TRUE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  VST3_CATEGORIES Fx PitchShift
  COPY_PLUGIN_AFTER_BUILD FALSE)
target_sources(NoteShaper PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp)
target_compile_definitions(NoteShaper PUBLIC
  JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0
  JUCE_DISPLAY_SPLASH_SCREEN=0 JUCE_REPORT_APP_USAGE=0)
target_link_libraries(NoteShaper PRIVATE
  juce::juce_audio_utils juce::juce_dsp
  PUBLIC juce::juce_recommended_config_flags)
if(MINGW)
  target_link_options(NoteShaper PUBLIC -static -static-libgcc -static-libstdc++)
endif()
add_executable(NoteShaperDSPTest Tests/dsp_test.cpp)
target_include_directories(NoteShaperDSPTest PRIVATE Source)
if(MINGW)
  target_link_options(NoteShaperDSPTest PRIVATE -static -static-libgcc -static-libstdc++)
endif()
if(DEFINED HOST_CHECK_SOURCE)
  add_executable(NoteShaperHostCheck "${HOST_CHECK_SOURCE}")
  target_compile_definitions(NoteShaperHostCheck PRIVATE
    JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_PLUGINHOST_VST3=1 JUCE_PLUGINHOST_VST=0
    JUCE_DISPLAY_SPLASH_SCREEN=0 JUCE_REPORT_APP_USAGE=0)
  target_link_libraries(NoteShaperHostCheck PRIVATE juce::juce_audio_utils juce::juce_recommended_config_flags)
  if(MINGW)
    target_link_options(NoteShaperHostCheck PRIVATE -static -static-libgcc -static-libstdc++)
  endif()
  add_executable(NoteShaperPreview Tests/preview.cpp)
  target_include_directories(NoteShaperPreview PRIVATE Source "${JUCE_PATH}/modules")
  target_compile_definitions(NoteShaperPreview PRIVATE
    JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1 JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0
    JUCE_DISPLAY_SPLASH_SCREEN=0 JUCE_REPORT_APP_USAGE=0)
  target_link_libraries(NoteShaperPreview PRIVATE NoteShaper)
  if(MINGW)
    target_link_options(NoteShaperPreview PRIVATE -static -static-libgcc -static-libstdc++)
  endif()
endif()
