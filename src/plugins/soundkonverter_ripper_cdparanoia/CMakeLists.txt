project(soundkonverter_ripper_cdparanoia)

find_package(Qt5Widgets)
find_package(Qt5Xml)

find_package(KF5I18n)

set(soundkonverter_ripper_cdparanoia_SRCS
   soundkonverter_ripper_cdparanoia.cpp
)

add_library(soundkonverter_ripper_cdparanoia STATIC ${soundkonverter_ripper_cdparanoia_SRCS})

target_link_libraries(soundkonverter_ripper_cdparanoia Qt5::Widgets Qt5::Xml KF5::I18n soundkonvertercore)

kservice_desktop_to_json(soundkonverter_ripper_cdparanoia soundkonverter_ripper_cdparanoia.desktop)

# install(TARGETS soundkonverter_ripper_cdparanoia DESTINATION ${PLUGIN_INSTALL_DIR})
# install(FILES soundkonverter_ripper_cdparanoia.desktop DESTINATION ${SERVICES_INSTALL_DIR})
