project(soundkonverter_ripper_cdparanoia)

if (KF5_BUILD)
    find_package(KF5 REQUIRED COMPONENTS WidgetsAddons)
else ()
    find_package(KDE4 REQUIRED)
    include (KDE4Defaults)
    include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )
endif ()

set(soundkonverter_ripper_cdparanoia_SRCS
   soundkonverter_ripper_cdparanoia.cpp
 )

if (KF5_BUILD)
    add_library(soundkonverter_ripper_cdparanoia MODULE ${soundkonverter_ripper_cdparanoia_SRCS})
    target_link_libraries(soundkonverter_ripper_cdparanoia KF5::WidgetsAddons soundkonvertercore )
else ()
    kde4_add_plugin(soundkonverter_ripper_cdparanoia ${soundkonverter_ripper_cdparanoia_SRCS})
    target_link_libraries(soundkonverter_ripper_cdparanoia ${KDE4_KDEUI_LIBS} soundkonvertercore )
endif ()

########### install files ###############

install(TARGETS soundkonverter_ripper_cdparanoia DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_ripper_cdparanoia.desktop DESTINATION ${SERVICES_INSTALL_DIR})
