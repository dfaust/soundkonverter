project(soundkonverter_replaygain_vorbisgain)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_replaygain_vorbisgain_SRCS
   soundkonverter_replaygain_vorbisgain.cpp
 )

kde4_add_plugin(soundkonverter_replaygain_vorbisgain ${soundkonverter_replaygain_vorbisgain_SRCS})

target_link_libraries(soundkonverter_replaygain_vorbisgain ${KDE4_KDEUI_LIBS} soundkonvertercore )

########### install files ###############

install(TARGETS soundkonverter_replaygain_vorbisgain DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_replaygain_vorbisgain.desktop DESTINATION ${SERVICES_INSTALL_DIR})
