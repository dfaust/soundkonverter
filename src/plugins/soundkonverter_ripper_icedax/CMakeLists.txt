project(soundkonverter_ripper_icedax)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_ripper_icedax_SRCS
   soundkonverter_ripper_icedax.cpp
 )

kde4_add_plugin(soundkonverter_ripper_icedax ${soundkonverter_ripper_icedax_SRCS})

target_link_libraries(soundkonverter_ripper_icedax ${KDE4_KDEUI_LIBS} soundkonvertercore )

########### install files ###############

install(TARGETS soundkonverter_ripper_icedax DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_ripper_icedax.desktop DESTINATION ${SERVICES_INSTALL_DIR})
