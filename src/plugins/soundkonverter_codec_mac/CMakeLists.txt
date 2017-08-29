project(soundkonverter_codec_mac)

find_package(KF5 REQUIRED COMPONENTS WidgetsAddons)
find_package(Qt5 REQUIRED COMPONENTS Xml)

set(soundkonverter_codec_mac_SRCS
   soundkonverter_codec_mac.cpp
   maccodecwidget.cpp
)

add_library(soundkonverter_codec_mac MODULE ${soundkonverter_codec_mac_SRCS})
target_link_libraries(soundkonverter_codec_mac KF5::WidgetsAddons Qt5::Xml soundkonvertercore)

########### install files ###############

install(TARGETS soundkonverter_codec_mac DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_mac.desktop DESTINATION ${SERVICES_INSTALL_DIR})
