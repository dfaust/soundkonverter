project(soundkonverter_codec_lame)

set(soundkonverter_codec_lame_SRCS
   soundkonverter_codec_lame.cpp
   lameconversionoptions.cpp
   lamecodecwidget.cpp
)

ki18n_wrap_ui(soundkonverter_codec_lame_SRCS
    lameconfigdialog.ui
)

add_library(soundkonverter_codec_lame SHARED ${soundkonverter_codec_lame_SRCS})

target_link_libraries(soundkonverter_codec_lame Qt5::Xml Qt5::Widgets Qt5::Xml KF5::I18n KF5::ConfigCore soundkonvertercore)

kservice_desktop_to_json(soundkonverter_codec_lame soundkonverter_codec_lame.desktop)

# install(TARGETS soundkonverter_codec_lame DESTINATION ${PLUGIN_INSTALL_DIR})
# install(FILES soundkonverter_codec_lame.desktop DESTINATION ${SERVICES_INSTALL_DIR})
