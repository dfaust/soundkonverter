project(soundkonverter_codec_lame)

find_package(KF5 REQUIRED COMPONENTS WidgetsAddons)
find_package(Qt5 REQUIRED COMPONENTS Xml)

set(soundkonverter_codec_lame_SRCS
   soundkonverter_codec_lame.cpp
   lameconversionoptions.cpp
   lamecodecwidget.cpp
)

add_library(soundkonverter_codec_lame MODULE ${soundkonverter_codec_lame_SRCS})
target_link_libraries(soundkonverter_codec_lame KF5::WidgetsAddons Qt5::Xml soundkonvertercore)

########### install files ###############

install(TARGETS soundkonverter_codec_lame DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_lame.desktop DESTINATION ${SERVICES_INSTALL_DIR})
