project(soundkonverter_replaygain_metaflac)

find_package(Qt5Widgets)
find_package(Qt5Xml)

find_package(KF5I18n)

set(soundkonverter_replaygain_metaflac_SRCS
   soundkonverter_replaygain_metaflac.cpp
)

add_library(soundkonverter_replaygain_metaflac STATIC ${soundkonverter_replaygain_metaflac_SRCS})

target_link_libraries(soundkonverter_replaygain_metaflac Qt5::Widgets Qt5::Xml KF5::I18n soundkonvertercore)

kservice_desktop_to_json(soundkonverter_replaygain_metaflac soundkonverter_replaygain_metaflac.desktop)

# install(TARGETS soundkonverter_replaygain_metaflac DESTINATION ${PLUGIN_INSTALL_DIR})
# install(FILES soundkonverter_replaygain_metaflac.desktop DESTINATION ${SERVICES_INSTALL_DIR})
