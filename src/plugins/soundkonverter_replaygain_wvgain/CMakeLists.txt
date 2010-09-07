project(soundkonverter_replaygain_wvgain)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_replaygain_wvgain_SRCS
   soundkonverter_replaygain_wvgain.cpp
 )

kde4_add_plugin(soundkonverter_replaygain_wvgain ${soundkonverter_replaygain_wvgain_SRCS})

# kde4_add_library(soundkonverter_replaygain_wvgain SHARED ${soundkonverter_replaygain_wvgain_SRCS})

target_link_libraries(soundkonverter_replaygain_wvgain ${KDE4_KDEUI_LIBS} soundkonvertercore )
#target_link_libraries(soundkonverter_replaygain_wvgain ${KDE4_KIO_LIBS} )

########### install files ###############

install(TARGETS soundkonverter_replaygain_wvgain DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_replaygain_wvgain.desktop DESTINATION ${SERVICES_INSTALL_DIR})
