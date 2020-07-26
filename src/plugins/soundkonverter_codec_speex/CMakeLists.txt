project(soundkonverter_codec_speex)

set(soundkonverter_codec_speex_SRCS
   soundkonverter_codec_speex.cpp
   speexcodecwidget.cpp
)

add_library(soundkonverter_codec_speex MODULE ${soundkonverter_codec_speex_SRCS})
target_link_libraries(soundkonverter_codec_speex KF5::WidgetsAddons Qt5::Xml soundkonvertercore)

########### install files ###############

install(TARGETS soundkonverter_codec_speex DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_speex.desktop DESTINATION ${SERVICES_INSTALL_DIR})
