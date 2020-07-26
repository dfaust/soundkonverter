project(soundkonverter_codec_twolame)

set(soundkonverter_codec_twolame_SRCS
   soundkonverter_codec_twolame.cpp
   twolamecodecwidget.cpp
)

add_library(soundkonverter_codec_twolame MODULE ${soundkonverter_codec_twolame_SRCS})
target_link_libraries(soundkonverter_codec_twolame KF5::WidgetsAddons Qt5::Xml soundkonvertercore)

########### install files ###############

install(TARGETS soundkonverter_codec_twolame DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_twolame.desktop DESTINATION ${SERVICES_INSTALL_DIR})
