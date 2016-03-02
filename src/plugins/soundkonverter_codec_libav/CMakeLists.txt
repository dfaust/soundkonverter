project(soundkonverter_codec_libav)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_codec_libav_SRCS
   soundkonverter_codec_libav.cpp
   libavcodecwidget.cpp
 )

kde4_add_plugin(soundkonverter_codec_libav ${soundkonverter_codec_libav_SRCS})

target_link_libraries(soundkonverter_codec_libav ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY} soundkonvertercore )

########### install files ###############

install(TARGETS soundkonverter_codec_libav DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_libav.desktop DESTINATION ${SERVICES_INSTALL_DIR})
