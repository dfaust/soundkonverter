project(soundkonverter_codec_mplayer)
find_package(Qt5Xml)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_codec_mplayer_SRCS
   soundkonverter_codec_mplayer.cpp
 )

add_library(soundkonverter_codec_mplayer MODULE ${soundkonverter_codec_mplayer_SRCS})

target_link_libraries(soundkonverter_codec_mplayer ${KDE4_KDEUI_LIBS} Qt5::Xml soundkonvertercore )

########### install files ###############

install(TARGETS soundkonverter_codec_mplayer DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_mplayer.desktop DESTINATION ${SERVICES_INSTALL_DIR})
