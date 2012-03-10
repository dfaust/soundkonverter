project(soundkonverter_codec_speex)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_codec_speex_SRCS
   soundkonverter_codec_speex.cpp
   speexcodecwidget.cpp
 )

kde4_add_plugin(soundkonverter_codec_speex ${soundkonverter_codec_speex_SRCS})

target_link_libraries(soundkonverter_codec_speex ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY} soundkonvertercore )

########### install files ###############

install(TARGETS soundkonverter_codec_speex DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_speex.desktop DESTINATION ${SERVICES_INSTALL_DIR})
