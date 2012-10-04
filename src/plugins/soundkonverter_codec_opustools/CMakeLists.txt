project(soundkonverter_codec_opustools)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_codec_opustools_SRCS
   soundkonverter_codec_opustools.cpp
   opustoolscodecwidget.cpp
   opustoolsconversionoptions.cpp
 )

kde4_add_plugin(soundkonverter_codec_opustools ${soundkonverter_codec_opustools_SRCS})

target_link_libraries(soundkonverter_codec_opustools ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY} soundkonvertercore )

########### install files ###############

install(TARGETS soundkonverter_codec_opustools DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_opustools.desktop DESTINATION ${SERVICES_INSTALL_DIR})
