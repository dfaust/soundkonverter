project(soundkonverter_codec_speex)
find_package(Qt5Xml)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_codec_speex_SRCS
   soundkonverter_codec_speex.cpp
   speexcodecwidget.cpp
 )

add_library(soundkonverter_codec_speex MODULE ${soundkonverter_codec_speex_SRCS})

target_link_libraries(soundkonverter_codec_speex ${KDE4_KDEUI_LIBS} Qt5::Xml soundkonvertercore )

########### install files ###############

install(TARGETS soundkonverter_codec_speex DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_speex.desktop DESTINATION ${SERVICES_INSTALL_DIR})
