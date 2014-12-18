project(soundkonverter_ripper_icedax)

find_package(Qt5Widgets)
find_package(Qt5Xml)

find_package(KF5I18n)

set(soundkonverter_ripper_icedax_SRCS
   soundkonverter_ripper_icedax.cpp
)

add_library(soundkonverter_ripper_icedax STATIC ${soundkonverter_ripper_icedax_SRCS})

target_link_libraries(soundkonverter_ripper_icedax Qt5::Widgets Qt5::Xml KF5::I18n soundkonvertercore)

kservice_desktop_to_json(soundkonverter_ripper_icedax soundkonverter_ripper_icedax.desktop)

# install(TARGETS soundkonverter_ripper_icedax DESTINATION ${PLUGIN_INSTALL_DIR})
# install(FILES soundkonverter_ripper_icedax.desktop DESTINATION ${SERVICES_INSTALL_DIR})
