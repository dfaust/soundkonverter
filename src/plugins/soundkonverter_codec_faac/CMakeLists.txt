project(soundkonverter_codec_faac)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_codec_faac_SRCS
   soundkonverter_codec_faac.cpp
   faaccodecwidget.cpp
 )

kde4_add_plugin(soundkonverter_codec_faac ${soundkonverter_codec_faac_SRCS})

target_link_libraries(soundkonverter_codec_faac ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY} soundkonvertercore )

########### install files ###############

install(TARGETS soundkonverter_codec_faac DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_faac.desktop DESTINATION ${SERVICES_INSTALL_DIR})
