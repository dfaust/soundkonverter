project(soundkonverter_codec_libav)

find_package(Qt5Widgets)
find_package(Qt5Xml)

find_package(KF5I18n)

set(soundkonverter_codec_libav_SRCS
   soundkonverter_codec_libav.cpp
   libavcodecwidget.cpp
)

add_library(soundkonverter_codec_libav STATIC ${soundkonverter_codec_libav_SRCS})

target_link_libraries(soundkonverter_codec_libav Qt5::Xml Qt5::Widgets Qt5::Xml KF5::I18n soundkonvertercore)

kservice_desktop_to_json(soundkonverter_codec_libav soundkonverter_codec_libav.desktop)

# install(TARGETS soundkonverter_codec_libav DESTINATION ${PLUGIN_INSTALL_DIR})
# install(FILES soundkonverter_codec_libav.desktop DESTINATION ${SERVICES_INSTALL_DIR})
