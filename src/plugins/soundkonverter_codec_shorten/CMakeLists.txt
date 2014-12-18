project(soundkonverter_codec_shorten)

find_package(Qt5Widgets)
find_package(Qt5Xml)

find_package(KF5I18n)

set(soundkonverter_codec_shorten_SRCS
   soundkonverter_codec_shorten.cpp
   shortencodecwidget.cpp
)

add_library(soundkonverter_codec_shorten SHARED ${soundkonverter_codec_shorten_SRCS})

target_link_libraries(soundkonverter_codec_shorten Qt5::Xml Qt5::Widgets Qt5::Xml KF5::I18n soundkonvertercore)

kservice_desktop_to_json(soundkonverter_codec_shorten soundkonverter_codec_shorten.desktop)

# install(TARGETS soundkonverter_codec_shorten DESTINATION ${PLUGIN_INSTALL_DIR})
# install(FILES soundkonverter_codec_shorten.desktop DESTINATION ${SERVICES_INSTALL_DIR})
