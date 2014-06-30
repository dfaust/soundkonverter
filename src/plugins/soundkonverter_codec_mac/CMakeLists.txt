project(soundkonverter_codec_mac)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_codec_mac_SRCS
   soundkonverter_codec_mac.cpp
   maccodecwidget.cpp
 )

kde4_add_plugin(soundkonverter_codec_mac ${soundkonverter_codec_mac_SRCS})

target_link_libraries(soundkonverter_codec_mac ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY} soundkonvertercore )

########### install files ###############

install(TARGETS soundkonverter_codec_mac DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_mac.desktop DESTINATION ${SERVICES_INSTALL_DIR})
