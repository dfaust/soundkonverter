project(soundkonverter_codec_timidity)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_codec_timidity_SRCS
   soundkonverter_codec_timidity.cpp
   timiditycodecwidget.cpp
 )

kde4_add_plugin(soundkonverter_codec_timidity ${soundkonverter_codec_timidity_SRCS})

# kde4_add_library(soundkonverter_codec_timidity SHARED ${soundkonverter_codec_timidity_SRCS})

target_link_libraries(soundkonverter_codec_timidity ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY} soundkonvertercore )
#target_link_libraries(soundkonverter_codec_timidity ${KDE4_KIO_LIBS} )

########### install files ###############

install(TARGETS soundkonverter_codec_timidity DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_timidity.desktop DESTINATION ${SERVICES_INSTALL_DIR})
