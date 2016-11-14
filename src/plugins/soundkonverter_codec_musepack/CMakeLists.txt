project(soundkonverter_codec_musepack)

find_package(KF5 REQUIRED COMPONENTS WidgetsAddons)
find_package(Qt5 REQUIRED COMPONENTS Xml)

set(soundkonverter_codec_musepack_SRCS
   soundkonverter_codec_musepack.cpp
   musepackcodecwidget.cpp
   musepackconversionoptions.cpp
)

add_library(soundkonverter_codec_musepack MODULE ${soundkonverter_codec_musepack_SRCS})
target_link_libraries(soundkonverter_codec_musepack KF5::WidgetsAddons Qt5::Xml soundkonvertercore)

########### install files ###############

install(TARGETS soundkonverter_codec_musepack DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_musepack.desktop DESTINATION ${SERVICES_INSTALL_DIR})
