project(soundkonverter_ripper_icedax)

if (KF5_BUILD)
    find_package(KF5 REQUIRED COMPONENTS WidgetsAddons)
else ()
    find_package(KDE4 REQUIRED)
    include (KDE4Defaults)
    include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )
endif ()

set(soundkonverter_ripper_icedax_SRCS
   soundkonverter_ripper_icedax.cpp
 )

if (KF5_BUILD)
    add_library(soundkonverter_ripper_icedax ${soundkonverter_ripper_icedax_SRCS})
    target_link_libraries(soundkonverter_ripper_icedax KF5::WidgetsAddons soundkonvertercore )
else ()
    kde4_add_plugin(soundkonverter_ripper_icedax ${soundkonverter_ripper_icedax_SRCS})
    target_link_libraries(soundkonverter_ripper_icedax ${KDE4_KDEUI_LIBS} soundkonvertercore )
endif ()

########### install files ###############

install(TARGETS soundkonverter_ripper_icedax DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_ripper_icedax.desktop DESTINATION ${SERVICES_INSTALL_DIR})
