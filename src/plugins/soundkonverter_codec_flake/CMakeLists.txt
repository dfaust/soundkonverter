project(soundkonverter_codec_flake)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_codec_flake_SRCS
   soundkonverter_codec_flake.cpp
   flakecodecwidget.cpp
 )

kde4_add_plugin(soundkonverter_codec_flake ${soundkonverter_codec_flake_SRCS})

target_link_libraries(soundkonverter_codec_flake ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY} soundkonvertercore )

########### install files ###############

install(TARGETS soundkonverter_codec_flake DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_flake.desktop DESTINATION ${SERVICES_INSTALL_DIR})
