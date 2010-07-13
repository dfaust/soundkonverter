project(soundkonverter_codec_vorbistools)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_codec_vorbistools_SRCS
   soundkonverter_codec_vorbistools.cpp
   vorbistoolscodecwidget.cpp
 )

kde4_add_plugin(soundkonverter_codec_vorbistools ${soundkonverter_codec_vorbistools_SRCS})

# kde4_add_library(soundkonverter_codec_vorbistools SHARED ${soundkonverter_codec_vorbistools_SRCS})

target_link_libraries(soundkonverter_codec_vorbistools ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY} soundkonvertercore )
#target_link_libraries(soundkonverter_codec_vorbistools ${KDE4_KIO_LIBS} )

########### install files ###############

install(TARGETS soundkonverter_codec_vorbistools DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_vorbistools.desktop DESTINATION ${SERVICES_INSTALL_DIR})
