project(soundkonverter_codec_ffmpeg)

find_package(Qt5Widgets)
find_package(Qt5Xml)

find_package(KF5I18n)

set(soundkonverter_codec_ffmpeg_SRCS
   soundkonverter_codec_ffmpeg.cpp
   ffmpegcodecwidget.cpp
)

add_library(soundkonverter_codec_ffmpeg STATIC ${soundkonverter_codec_ffmpeg_SRCS})

target_link_libraries(soundkonverter_codec_ffmpeg Qt5::Xml Qt5::Widgets Qt5::Xml KF5::I18n soundkonvertercore)

kservice_desktop_to_json(soundkonverter_codec_ffmpeg soundkonverter_codec_ffmpeg.desktop)

# install(TARGETS soundkonverter_codec_ffmpeg DESTINATION ${PLUGIN_INSTALL_DIR})
# install(FILES soundkonverter_codec_ffmpeg.desktop DESTINATION ${SERVICES_INSTALL_DIR})
