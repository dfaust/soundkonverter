project(soundkonverter_ripper_cdparanoia)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_ripper_cdparanoia_SRCS
   soundkonverter_ripper_cdparanoia.cpp
 )

kde4_add_plugin(soundkonverter_ripper_cdparanoia ${soundkonverter_ripper_cdparanoia_SRCS})

target_link_libraries(soundkonverter_ripper_cdparanoia ${KDE4_KDEUI_LIBS} soundkonvertercore )

########### install files ###############

install(TARGETS soundkonverter_ripper_cdparanoia DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_ripper_cdparanoia.desktop DESTINATION ${SERVICES_INSTALL_DIR})
