project(soundkonverter_codec_flac)

find_package(Qt5Widgets)
find_package(Qt5Xml)

find_package(KF5I18n)

set(soundkonverter_codec_flac_SRCS
   soundkonverter_codec_flac.cpp
   flaccodecwidget.cpp
)

add_library(soundkonverter_codec_flac STATIC ${soundkonverter_codec_flac_SRCS})

target_link_libraries(soundkonverter_codec_flac Qt5::Widgets Qt5::Xml KF5::I18n soundkonvertercore)

kservice_desktop_to_json(soundkonverter_codec_flac soundkonverter_codec_flac.desktop)

# install(TARGETS soundkonverter_codec_flac DESTINATION ${PLUGIN_INSTALL_DIR})
# install(FILES soundkonverter_codec_flac.desktop DESTINATION ${SERVICES_INSTALL_DIR})
