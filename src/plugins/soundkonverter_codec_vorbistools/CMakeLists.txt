project(soundkonverter_codec_vorbistools)

find_package(Qt5Widgets)
find_package(Qt5Xml)

find_package(KF5I18n)

set(soundkonverter_codec_vorbistools_SRCS
   soundkonverter_codec_vorbistools.cpp
   vorbistoolscodecwidget.cpp
)

add_library(soundkonverter_codec_vorbistools SHARED ${soundkonverter_codec_vorbistools_SRCS})

target_link_libraries(soundkonverter_codec_vorbistools Qt5::Xml Qt5::Widgets Qt5::Xml KF5::I18n soundkonvertercore)

kservice_desktop_to_json(soundkonverter_codec_vorbistools soundkonverter_codec_vorbistools.desktop)

# install(TARGETS soundkonverter_codec_vorbistools DESTINATION ${PLUGIN_INSTALL_DIR})
# install(FILES soundkonverter_codec_vorbistools.desktop DESTINATION ${SERVICES_INSTALL_DIR})
