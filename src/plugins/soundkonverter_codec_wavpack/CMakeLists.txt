project(soundkonverter_codec_wavpack)

find_package(Qt5Widgets)
find_package(Qt5Xml)

find_package(KF5I18n)

set(soundkonverter_codec_wavpack_SRCS
   soundkonverter_codec_wavpack.cpp
   wavpackcodecwidget.cpp
)

add_library(soundkonverter_codec_wavpack SHARED ${soundkonverter_codec_wavpack_SRCS})

target_link_libraries(soundkonverter_codec_wavpack Qt5::Xml Qt5::Widgets Qt5::Xml KF5::I18n soundkonvertercore)

kservice_desktop_to_json(soundkonverter_codec_wavpack soundkonverter_codec_wavpack.desktop)

# install(TARGETS soundkonverter_codec_wavpack DESTINATION ${PLUGIN_INSTALL_DIR})
# install(FILES soundkonverter_codec_wavpack.desktop DESTINATION ${SERVICES_INSTALL_DIR})
