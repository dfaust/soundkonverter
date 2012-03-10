project(soundkonverter_codec_ffmpeg)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_codec_ffmpeg_SRCS
   soundkonverter_codec_ffmpeg.cpp
   ffmpegcodecwidget.cpp
 )

kde4_add_plugin(soundkonverter_codec_ffmpeg ${soundkonverter_codec_ffmpeg_SRCS})

target_link_libraries(soundkonverter_codec_ffmpeg ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY} soundkonvertercore )

########### install files ###############

install(TARGETS soundkonverter_codec_ffmpeg DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_ffmpeg.desktop DESTINATION ${SERVICES_INSTALL_DIR})
