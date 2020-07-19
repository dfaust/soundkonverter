project(soundkonverter_codec_fluidsynth)

set(soundkonverter_codec_fluidsynth_SRCS
   soundkonverter_codec_fluidsynth.cpp
   fluidsynthcodecwidget.cpp
)

add_library(soundkonverter_codec_fluidsynth MODULE ${soundkonverter_codec_fluidsynth_SRCS})
target_link_libraries(soundkonverter_codec_fluidsynth KF5::WidgetsAddons KF5::KIOCore Qt5::Xml soundkonvertercore)

########### install files ###############

install(TARGETS soundkonverter_codec_fluidsynth DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_fluidsynth.desktop DESTINATION ${SERVICES_INSTALL_DIR})
