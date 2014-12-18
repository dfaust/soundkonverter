project(soundkonverter_codec_musepack)

find_package(Qt5Widgets)
find_package(Qt5Xml)

find_package(KF5I18n)

set(soundkonverter_codec_musepack_SRCS
   soundkonverter_codec_musepack.cpp
   musepackcodecwidget.cpp
   musepackconversionoptions.cpp
)

add_library(soundkonverter_codec_musepack STATIC ${soundkonverter_codec_musepack_SRCS})

target_link_libraries(soundkonverter_codec_musepack Qt5::Xml Qt5::Widgets Qt5::Xml KF5::I18n soundkonvertercore)

kservice_desktop_to_json(soundkonverter_codec_musepack soundkonverter_codec_musepack.desktop)

# install(TARGETS soundkonverter_codec_musepack DESTINATION ${PLUGIN_INSTALL_DIR})
# install(FILES soundkonverter_codec_musepack.desktop DESTINATION ${SERVICES_INSTALL_DIR})
