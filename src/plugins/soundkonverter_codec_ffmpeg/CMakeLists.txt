project(soundkonverter_codec_ffmpeg)

set(soundkonverter_codec_ffmpeg_SRCS
   soundkonverter_codec_ffmpeg.cpp
   ffmpegcodecwidget.cpp
)

add_library(soundkonverter_codec_ffmpeg MODULE ${soundkonverter_codec_ffmpeg_SRCS})
target_link_libraries(soundkonverter_codec_ffmpeg KF5::WidgetsAddons Qt5::Xml soundkonvertercore)

########### install files ###############

install(TARGETS soundkonverter_codec_ffmpeg DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_ffmpeg.desktop DESTINATION ${SERVICES_INSTALL_DIR})
