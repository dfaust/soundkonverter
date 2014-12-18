project(soundkonverter_replaygain_vorbisgain)

find_package(Qt5Widgets)
find_package(Qt5Xml)

find_package(KF5I18n)

set(soundkonverter_replaygain_vorbisgain_SRCS
   soundkonverter_replaygain_vorbisgain.cpp
)

add_library(soundkonverter_replaygain_vorbisgain SHARED ${soundkonverter_replaygain_vorbisgain_SRCS})

target_link_libraries(soundkonverter_replaygain_vorbisgain Qt5::Widgets Qt5::Xml KF5::I18n soundkonvertercore)

kservice_desktop_to_json(soundkonverter_replaygain_vorbisgain soundkonverter_replaygain_vorbisgain.desktop)

# install(TARGETS soundkonverter_replaygain_vorbisgain DESTINATION ${PLUGIN_INSTALL_DIR})
# install(FILES soundkonverter_replaygain_vorbisgain.desktop DESTINATION ${SERVICES_INSTALL_DIR})
