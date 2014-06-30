project(soundkonverter_replaygain_musepackgain)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_replaygain_musepackgain_SRCS
   soundkonverter_replaygain_musepackgain.cpp
 )

kde4_add_plugin(soundkonverter_replaygain_musepackgain ${soundkonverter_replaygain_musepackgain_SRCS})

target_link_libraries(soundkonverter_replaygain_musepackgain ${KDE4_KDEUI_LIBS} soundkonvertercore )

########### install files ###############

install(TARGETS soundkonverter_replaygain_musepackgain DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_replaygain_musepackgain.desktop DESTINATION ${SERVICES_INSTALL_DIR})
