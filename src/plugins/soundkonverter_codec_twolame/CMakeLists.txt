project(soundkonverter_codec_twolame)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_codec_twolame_SRCS
   soundkonverter_codec_twolame.cpp
   twolamecodecwidget.cpp
 )

kde4_add_plugin(soundkonverter_codec_twolame ${soundkonverter_codec_twolame_SRCS})

# kde4_add_library(soundkonverter_codec_twolame SHARED ${soundkonverter_codec_twolame_SRCS})

target_link_libraries(soundkonverter_codec_twolame ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY} soundkonvertercore )
#target_link_libraries(soundkonverter_codec_twolame ${KDE4_KIO_LIBS} )

########### install files ###############

install(TARGETS soundkonverter_codec_twolame DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_twolame.desktop DESTINATION ${SERVICES_INSTALL_DIR})
