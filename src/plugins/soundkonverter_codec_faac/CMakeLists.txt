project(soundkonverter_codec_faac)

find_package(Qt5Widgets)
find_package(Qt5Xml)

find_package(KF5I18n)

set(soundkonverter_codec_faac_SRCS
   soundkonverter_codec_faac.cpp
   faaccodecwidget.cpp
)

add_library(soundkonverter_codec_faac STATIC ${soundkonverter_codec_faac_SRCS})

target_link_libraries(soundkonverter_codec_faac Qt5::Xml Qt5::Widgets Qt5::Xml KF5::I18n soundkonvertercore)

kservice_desktop_to_json(soundkonverter_codec_faac soundkonverter_codec_faac.desktop)

# install(TARGETS soundkonverter_codec_faac DESTINATION ${PLUGIN_INSTALL_DIR})
# install(FILES soundkonverter_codec_faac.desktop DESTINATION ${SERVICES_INSTALL_DIR})
