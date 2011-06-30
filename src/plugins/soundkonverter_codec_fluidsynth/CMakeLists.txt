project(soundkonverter_codec_fluidsynth)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_codec_fluidsynth_SRCS
   soundkonverter_codec_fluidsynth.cpp
   fluidsynthcodecwidget.cpp
 )

kde4_add_plugin(soundkonverter_codec_fluidsynth ${soundkonverter_codec_fluidsynth_SRCS})

target_link_libraries(soundkonverter_codec_fluidsynth ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} ${QT_QTXML_LIBRARY} soundkonvertercore )

########### install files ###############

install(TARGETS soundkonverter_codec_fluidsynth DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_fluidsynth.desktop DESTINATION ${SERVICES_INSTALL_DIR})
