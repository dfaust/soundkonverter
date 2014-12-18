project(soundkonverter_replaygain_wvgain)

find_package(Qt5Widgets)
find_package(Qt5Xml)

find_package(KF5I18n)

set(soundkonverter_replaygain_wvgain_SRCS
   soundkonverter_replaygain_wvgain.cpp
)

add_library(soundkonverter_replaygain_wvgain SHARED ${soundkonverter_replaygain_wvgain_SRCS})

target_link_libraries(soundkonverter_replaygain_wvgain Qt5::Widgets Qt5::Xml KF5::I18n soundkonvertercore)

kservice_desktop_to_json(soundkonverter_replaygain_wvgain soundkonverter_replaygain_wvgain.desktop)

# install(TARGETS soundkonverter_replaygain_wvgain DESTINATION ${PLUGIN_INSTALL_DIR})
# install(FILES soundkonverter_replaygain_wvgain.desktop DESTINATION ${SERVICES_INSTALL_DIR})
