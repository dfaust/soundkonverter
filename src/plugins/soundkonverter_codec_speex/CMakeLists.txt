project(soundkonverter_codec_speex)

find_package(Qt5Widgets)
find_package(Qt5Xml)

find_package(KF5I18n)

set(soundkonverter_codec_speex_SRCS
   soundkonverter_codec_speex.cpp
   speexcodecwidget.cpp
)

add_library(soundkonverter_codec_speex STATIC ${soundkonverter_codec_speex_SRCS})

target_link_libraries(soundkonverter_codec_speex Qt5::Xml Qt5::Widgets Qt5::Xml KF5::I18n soundkonvertercore)

kservice_desktop_to_json(soundkonverter_codec_speex soundkonverter_codec_speex.desktop)

# install(TARGETS soundkonverter_codec_speex DESTINATION ${PLUGIN_INSTALL_DIR})
# install(FILES soundkonverter_codec_speex.desktop DESTINATION ${SERVICES_INSTALL_DIR})
