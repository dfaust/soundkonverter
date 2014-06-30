project(soundkonverter_codec_neroaac)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_codec_neroaac_SRCS
   soundkonverter_codec_neroaac.cpp
   neroaaccodecwidget.cpp
 )

kde4_add_plugin(soundkonverter_codec_neroaac ${soundkonverter_codec_neroaac_SRCS})

target_link_libraries(soundkonverter_codec_neroaac ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY} soundkonvertercore )

########### install files ###############

install(TARGETS soundkonverter_codec_neroaac DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_neroaac.desktop DESTINATION ${SERVICES_INSTALL_DIR})
