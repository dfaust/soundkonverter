project(soundkonverter_codec_libav)

set(soundkonverter_codec_libav_SRCS
   soundkonverter_codec_libav.cpp
   libavcodecwidget.cpp
)

add_library(soundkonverter_codec_libav MODULE ${soundkonverter_codec_libav_SRCS})
target_link_libraries(soundkonverter_codec_libav KF5::WidgetsAddons Qt5::Xml soundkonvertercore)

########### install files ###############

install(TARGETS soundkonverter_codec_libav DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_libav.desktop DESTINATION ${SERVICES_INSTALL_DIR})
