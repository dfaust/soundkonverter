project(soundkonverter_codec_fluidsynth)

find_package(Qt5Widgets)
find_package(Qt5Xml)

find_package(KF5I18n)
find_package(KF5KIO)

set(soundkonverter_codec_fluidsynth_SRCS
   soundkonverter_codec_fluidsynth.cpp
   fluidsynthcodecwidget.cpp
)

add_library(soundkonverter_codec_fluidsynth STATIC ${soundkonverter_codec_fluidsynth_SRCS})

target_link_libraries(soundkonverter_codec_fluidsynth Qt5::Xml Qt5::Widgets Qt5::Xml KF5::I18n KF5::KIOFileWidgets soundkonvertercore)

kservice_desktop_to_json(soundkonverter_codec_fluidsynth soundkonverter_codec_fluidsynth.desktop)

# install(TARGETS soundkonverter_codec_fluidsynth DESTINATION ${PLUGIN_INSTALL_DIR})
# install(FILES soundkonverter_codec_fluidsynth.desktop DESTINATION ${SERVICES_INSTALL_DIR})
