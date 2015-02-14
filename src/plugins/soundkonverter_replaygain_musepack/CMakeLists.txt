project(soundkonverter_replaygain_musepackgain)

if (KF5_BUILD)
    find_package(KF5 REQUIRED COMPONENTS WidgetsAddons)
else ()
    find_package(KDE4 REQUIRED)
    include (KDE4Defaults)
    include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )
endif ()

set(soundkonverter_replaygain_musepackgain_SRCS
   soundkonverter_replaygain_musepackgain.cpp
 )

if (KF5_BUILD)
    add_library(soundkonverter_replaygain_musepackgain ${soundkonverter_replaygain_musepackgain_SRCS})
    target_link_libraries(soundkonverter_replaygain_musepackgain KF5::WidgetsAddons soundkonvertercore )
else ()
    kde4_add_plugin(soundkonverter_replaygain_musepackgain ${soundkonverter_replaygain_musepackgain_SRCS})
    target_link_libraries(soundkonverter_replaygain_musepackgain ${KDE4_KDEUI_LIBS} soundkonvertercore )
endif ()

########### install files ###############

install(TARGETS soundkonverter_replaygain_musepackgain DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_replaygain_musepackgain.desktop DESTINATION ${SERVICES_INSTALL_DIR})
