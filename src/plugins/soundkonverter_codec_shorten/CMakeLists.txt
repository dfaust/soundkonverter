project(soundkonverter_codec_shorten)

find_package(KF5 REQUIRED COMPONENTS WidgetsAddons)
find_package(Qt5 REQUIRED COMPONENTS Xml)

set(soundkonverter_codec_shorten_SRCS
   soundkonverter_codec_shorten.cpp
   shortencodecwidget.cpp
)

add_library(soundkonverter_codec_shorten MODULE ${soundkonverter_codec_shorten_SRCS})
target_link_libraries(soundkonverter_codec_shorten KF5::WidgetsAddons Qt5::Xml soundkonvertercore)

########### install files ###############

install(TARGETS soundkonverter_codec_shorten DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_shorten.desktop DESTINATION ${SERVICES_INSTALL_DIR})
