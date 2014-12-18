project(soundkonverter_codec_lame)

find_package(Qt5Widgets)
find_package(Qt5Xml)

find_package(KF5I18n)

set(soundkonverter_codec_lame_SRCS
   soundkonverter_codec_lame.cpp
   lameconversionoptions.cpp
   lamecodecwidget.cpp
)

add_library(soundkonverter_codec_lame STATIC ${soundkonverter_codec_lame_SRCS})

target_link_libraries(soundkonverter_codec_lame Qt5::Xml Qt5::Widgets Qt5::Xml KF5::I18n soundkonvertercore)

kservice_desktop_to_json(soundkonverter_codec_lame soundkonverter_codec_lame.desktop)

# install(TARGETS soundkonverter_codec_lame DESTINATION ${PLUGIN_INSTALL_DIR})
# install(FILES soundkonverter_codec_lame.desktop DESTINATION ${SERVICES_INSTALL_DIR})
