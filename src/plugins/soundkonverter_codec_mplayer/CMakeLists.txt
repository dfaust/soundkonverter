project(soundkonverter_codec_mplayer)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_codec_mplayer_SRCS
   soundkonverter_codec_mplayer.cpp
 )

kde4_add_plugin(soundkonverter_codec_mplayer ${soundkonverter_codec_mplayer_SRCS})

target_link_libraries(soundkonverter_codec_mplayer ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY} soundkonvertercore )

########### install files ###############

install(TARGETS soundkonverter_codec_mplayer DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_mplayer.desktop DESTINATION ${SERVICES_INSTALL_DIR})
