project(soundkonverter_codec_aften)

set(soundkonverter_codec_aften_SRCS
   soundkonverter_codec_aften.cpp
   aftencodecwidget.cpp
)

add_library(soundkonverter_codec_aften MODULE ${soundkonverter_codec_aften_SRCS})
target_link_libraries(soundkonverter_codec_aften KF5::WidgetsAddons Qt5::Xml soundkonvertercore)

########### install files ###############

install(TARGETS soundkonverter_codec_aften DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_aften.desktop DESTINATION ${SERVICES_INSTALL_DIR})
