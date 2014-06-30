project(soundkonverter_replaygain_mp3gain)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_replaygain_mp3gain_SRCS
   soundkonverter_replaygain_mp3gain.cpp
 )

kde4_add_plugin(soundkonverter_replaygain_mp3gain ${soundkonverter_replaygain_mp3gain_SRCS})

target_link_libraries(soundkonverter_replaygain_mp3gain ${KDE4_KDEUI_LIBS} soundkonvertercore )

########### install files ###############

install(TARGETS soundkonverter_replaygain_mp3gain DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_replaygain_mp3gain.desktop DESTINATION ${SERVICES_INSTALL_DIR})
