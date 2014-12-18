project(soundkonverter_replaygain_mp3gain)

find_package(Qt5Widgets)
find_package(Qt5Xml)

find_package(KF5I18n)

set(soundkonverter_replaygain_mp3gain_SRCS
   soundkonverter_replaygain_mp3gain.cpp
)

add_library(soundkonverter_replaygain_mp3gain STATIC ${soundkonverter_replaygain_mp3gain_SRCS})

target_link_libraries(soundkonverter_replaygain_mp3gain Qt5::Widgets Qt5::Xml KF5::I18n soundkonvertercore)

kservice_desktop_to_json(soundkonverter_replaygain_mp3gain soundkonverter_replaygain_mp3gain.desktop)

# install(TARGETS soundkonverter_replaygain_mp3gain DESTINATION ${PLUGIN_INSTALL_DIR})
# install(FILES soundkonverter_replaygain_mp3gain.desktop DESTINATION ${SERVICES_INSTALL_DIR})
