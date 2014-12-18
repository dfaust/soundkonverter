project(soundkonverter_codec_mac)

find_package(Qt5Widgets)
find_package(Qt5Xml)

find_package(KF5I18n)

set(soundkonverter_codec_mac_SRCS
   soundkonverter_codec_mac.cpp
   maccodecwidget.cpp
)

add_library(soundkonverter_codec_mac STATIC ${soundkonverter_codec_mac_SRCS})

target_link_libraries(soundkonverter_codec_mac Qt5::Xml Qt5::Widgets Qt5::Xml KF5::I18n soundkonvertercore)

kservice_desktop_to_json(soundkonverter_codec_mac soundkonverter_codec_mac.desktop)

# install(TARGETS soundkonverter_codec_mac DESTINATION ${PLUGIN_INSTALL_DIR})
# install(FILES soundkonverter_codec_mac.desktop DESTINATION ${SERVICES_INSTALL_DIR})
