project(soundkonverter_codec_lame)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_codec_lame_SRCS
   soundkonverter_codec_lame.cpp
   lameconversionoptions.cpp
   lamecodecwidget.cpp
 )

kde4_add_plugin(soundkonverter_codec_lame ${soundkonverter_codec_lame_SRCS})

# kde4_add_library(soundkonverter_codec_lame SHARED ${soundkonverter_codec_lame_SRCS})

target_link_libraries(soundkonverter_codec_lame ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY} soundkonvertercore )
#target_link_libraries(soundkonverter_codec_lame ${KDE4_KIO_LIBS} )

########### install files ###############

install(TARGETS soundkonverter_codec_lame DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_lame.desktop DESTINATION ${SERVICES_INSTALL_DIR})
