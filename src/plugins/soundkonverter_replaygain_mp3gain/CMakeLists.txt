project(soundkonverter_replaygain_mp3gain)

if (KF5_BUILD)
    find_package(KF5 REQUIRED COMPONENTS WidgetsAddons)
else ()
    find_package(KDE4 REQUIRED)
    include (KDE4Defaults)
    include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )
endif ()

set(soundkonverter_replaygain_mp3gain_SRCS
   soundkonverter_replaygain_mp3gain.cpp
 )

if (KF5_BUILD)
    add_library(soundkonverter_replaygain_mp3gain ${soundkonverter_replaygain_mp3gain_SRCS})
    target_link_libraries(soundkonverter_replaygain_mp3gain KF5::WidgetsAddons soundkonvertercore )
else ()
    kde4_add_plugin(soundkonverter_replaygain_mp3gain ${soundkonverter_replaygain_mp3gain_SRCS})
    target_link_libraries(soundkonverter_replaygain_mp3gain ${KDE4_KDEUI_LIBS} soundkonvertercore )
endif ()

########### install files ###############

install(TARGETS soundkonverter_replaygain_mp3gain DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_replaygain_mp3gain.desktop DESTINATION ${SERVICES_INSTALL_DIR})
