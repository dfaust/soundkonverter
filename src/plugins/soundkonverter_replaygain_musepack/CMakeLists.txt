project(soundkonverter_replaygain_musepackgain)

find_package(Qt5Widgets)
find_package(Qt5Xml)

find_package(KF5I18n)

set(soundkonverter_replaygain_musepackgain_SRCS
   soundkonverter_replaygain_musepackgain.cpp
)

add_library(soundkonverter_replaygain_musepackgain STATIC ${soundkonverter_replaygain_musepackgain_SRCS})

target_link_libraries(soundkonverter_replaygain_musepackgain Qt5::Widgets Qt5::Xml KF5::I18n soundkonvertercore)

kservice_desktop_to_json(soundkonverter_replaygain_musepackgain soundkonverter_replaygain_musepackgain.desktop)

# install(TARGETS soundkonverter_replaygain_musepackgain DESTINATION ${PLUGIN_INSTALL_DIR})
# install(FILES soundkonverter_replaygain_musepackgain.desktop DESTINATION ${SERVICES_INSTALL_DIR})
