project(soundkonverter_codec_ttaenc)
find_package(Qt5Xml)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_codec_ttaenc_SRCS
   soundkonverter_codec_ttaenc.cpp
   ttaenccodecwidget.cpp
 )

add_library(soundkonverter_codec_ttaenc MODULE ${soundkonverter_codec_ttaenc_SRCS})

target_link_libraries(soundkonverter_codec_ttaenc ${KDE4_KDEUI_LIBS} Qt5::Xml soundkonvertercore )

########### install files ###############

install(TARGETS soundkonverter_codec_ttaenc DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_ttaenc.desktop DESTINATION ${SERVICES_INSTALL_DIR})
