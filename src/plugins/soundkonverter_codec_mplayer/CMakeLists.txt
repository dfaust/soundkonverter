project(soundkonverter_codec_mplayer)

find_package(Qt5Widgets)
find_package(Qt5Xml)

find_package(KF5I18n)

set(soundkonverter_codec_mplayer_SRCS
   soundkonverter_codec_mplayer.cpp
)

add_library(soundkonverter_codec_mplayer STATIC ${soundkonverter_codec_mplayer_SRCS})

target_link_libraries(soundkonverter_codec_mplayer Qt5::Xml Qt5::Widgets Qt5::Xml KF5::I18n soundkonvertercore)

kservice_desktop_to_json(soundkonverter_codec_mplayer soundkonverter_codec_mplayer.desktop)

# install(TARGETS soundkonverter_codec_mplayer DESTINATION ${PLUGIN_INSTALL_DIR})
# install(FILES soundkonverter_codec_mplayer.desktop DESTINATION ${SERVICES_INSTALL_DIR})
