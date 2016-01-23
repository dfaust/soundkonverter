project(soundkonverter_replaygain_metaflac)

if (KF5_BUILD)
    find_package(KF5 REQUIRED COMPONENTS WidgetsAddons)
else ()
    find_package(KDE4 REQUIRED)
    include (KDE4Defaults)
    include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )
endif ()

set(soundkonverter_replaygain_metaflac_SRCS
   soundkonverter_replaygain_metaflac.cpp
 )

if (KF5_BUILD)
    add_library(soundkonverter_replaygain_metaflac MODULE ${soundkonverter_replaygain_metaflac_SRCS})
    target_link_libraries(soundkonverter_replaygain_metaflac KF5::WidgetsAddons soundkonvertercore )
else ()
    kde4_add_plugin(soundkonverter_replaygain_metaflac ${soundkonverter_replaygain_metaflac_SRCS})
    target_link_libraries(soundkonverter_replaygain_metaflac ${KDE4_KDEUI_LIBS} soundkonvertercore )
endif ()

########### install files ###############

install(TARGETS soundkonverter_replaygain_metaflac DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_replaygain_metaflac.desktop DESTINATION ${SERVICES_INSTALL_DIR})
