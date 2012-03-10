project(soundkonverter_codec_shorten)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_codec_shorten_SRCS
   soundkonverter_codec_shorten.cpp
   shortencodecwidget.cpp
 )

kde4_add_plugin(soundkonverter_codec_shorten ${soundkonverter_codec_shorten_SRCS})

target_link_libraries(soundkonverter_codec_shorten ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY} soundkonvertercore )

########### install files ###############

install(TARGETS soundkonverter_codec_shorten DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_shorten.desktop DESTINATION ${SERVICES_INSTALL_DIR})
