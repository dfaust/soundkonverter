project(soundkonverter_codec_neroaac)

find_package(Qt5Widgets)
find_package(Qt5Xml)

find_package(KF5I18n)

set(soundkonverter_codec_neroaac_SRCS
   soundkonverter_codec_neroaac.cpp
   neroaaccodecwidget.cpp
)

add_library(soundkonverter_codec_neroaac SHARED ${soundkonverter_codec_neroaac_SRCS})

target_link_libraries(soundkonverter_codec_neroaac Qt5::Xml Qt5::Widgets Qt5::Xml KF5::I18n soundkonvertercore)

kservice_desktop_to_json(soundkonverter_codec_neroaac soundkonverter_codec_neroaac.desktop)

# install(TARGETS soundkonverter_codec_neroaac DESTINATION ${PLUGIN_INSTALL_DIR})
# install(FILES soundkonverter_codec_neroaac.desktop DESTINATION ${SERVICES_INSTALL_DIR})
