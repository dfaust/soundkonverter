project(soundkonverter_codec_flake)

find_package(KF5 REQUIRED COMPONENTS WidgetsAddons)
find_package(Qt5 REQUIRED COMPONENTS Xml)

set(soundkonverter_codec_flake_SRCS
   soundkonverter_codec_flake.cpp
   flakecodecwidget.cpp
)

add_library(soundkonverter_codec_flake MODULE ${soundkonverter_codec_flake_SRCS})
target_link_libraries(soundkonverter_codec_flake KF5::WidgetsAddons Qt5::Xml soundkonvertercore)

########### install files ###############

install(TARGETS soundkonverter_codec_flake DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_flake.desktop DESTINATION ${SERVICES_INSTALL_DIR})
