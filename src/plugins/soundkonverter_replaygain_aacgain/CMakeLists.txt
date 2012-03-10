project(soundkonverter_replaygain_aacgain)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_replaygain_aacgain_SRCS
   soundkonverter_replaygain_aacgain.cpp
 )

kde4_add_plugin(soundkonverter_replaygain_aacgain ${soundkonverter_replaygain_aacgain_SRCS})

target_link_libraries(soundkonverter_replaygain_aacgain ${KDE4_KDEUI_LIBS} soundkonvertercore )

########### install files ###############

install(TARGETS soundkonverter_replaygain_aacgain DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_replaygain_aacgain.desktop DESTINATION ${SERVICES_INSTALL_DIR})
