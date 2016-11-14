project(soundkonverter_ripper_cdparanoia)

find_package(KF5 REQUIRED COMPONENTS WidgetsAddons)

set(soundkonverter_ripper_cdparanoia_SRCS
   soundkonverter_ripper_cdparanoia.cpp
)

add_library(soundkonverter_ripper_cdparanoia MODULE ${soundkonverter_ripper_cdparanoia_SRCS})
target_link_libraries(soundkonverter_ripper_cdparanoia KF5::WidgetsAddons soundkonvertercore)

########### install files ###############

install(TARGETS soundkonverter_ripper_cdparanoia DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_ripper_cdparanoia.desktop DESTINATION ${SERVICES_INSTALL_DIR})
