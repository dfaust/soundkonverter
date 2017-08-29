project(soundkonverter_codec_mplayer)

find_package(KF5 REQUIRED COMPONENTS WidgetsAddons)
find_package(Qt5 REQUIRED COMPONENTS Xml)

set(soundkonverter_codec_mplayer_SRCS
   soundkonverter_codec_mplayer.cpp
)

add_library(soundkonverter_codec_mplayer MODULE ${soundkonverter_codec_mplayer_SRCS})
target_link_libraries(soundkonverter_codec_mplayer KF5::WidgetsAddons Qt5::Xml soundkonvertercore)

########### install files ###############

install(TARGETS soundkonverter_codec_mplayer DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_mplayer.desktop DESTINATION ${SERVICES_INSTALL_DIR})
