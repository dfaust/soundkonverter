project(soundkonverter_codec_ttaenc)

set(soundkonverter_codec_ttaenc_SRCS
   soundkonverter_codec_ttaenc.cpp
   ttaenccodecwidget.cpp
)

add_library(soundkonverter_codec_ttaenc MODULE ${soundkonverter_codec_ttaenc_SRCS})
target_link_libraries(soundkonverter_codec_ttaenc KF5::WidgetsAddons Qt5::Xml soundkonvertercore)

########### install files ###############

install(TARGETS soundkonverter_codec_ttaenc DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_ttaenc.desktop DESTINATION ${SERVICES_INSTALL_DIR})
