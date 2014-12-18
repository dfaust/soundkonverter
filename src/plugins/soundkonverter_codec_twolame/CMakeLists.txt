project(soundkonverter_codec_twolame)

find_package(Qt5Widgets)
find_package(Qt5Xml)

find_package(KF5I18n)

set(soundkonverter_codec_twolame_SRCS
   soundkonverter_codec_twolame.cpp
   twolamecodecwidget.cpp
)

add_library(soundkonverter_codec_twolame SHARED ${soundkonverter_codec_twolame_SRCS})

target_link_libraries(soundkonverter_codec_twolame Qt5::Xml Qt5::Widgets Qt5::Xml KF5::I18n soundkonvertercore)

kservice_desktop_to_json(soundkonverter_codec_twolame soundkonverter_codec_twolame.desktop)

# install(TARGETS soundkonverter_codec_twolame DESTINATION ${PLUGIN_INSTALL_DIR})
# install(FILES soundkonverter_codec_twolame.desktop DESTINATION ${SERVICES_INSTALL_DIR})
