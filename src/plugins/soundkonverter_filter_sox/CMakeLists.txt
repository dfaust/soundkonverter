project(soundkonverter_filter_sox)

find_package(Qt5Widgets)
find_package(Qt5Xml)

find_package(KF5I18n)

set(soundkonverter_filter_sox_SRCS
   soundkonverter_filter_sox.cpp
   soxfilteroptions.cpp
   soxcodecwidget.cpp
   soxfilterwidget.cpp
   soxeffectwidget.cpp
)

add_library(soundkonverter_filter_sox STATIC ${soundkonverter_filter_sox_SRCS})

target_link_libraries(soundkonverter_filter_sox Qt5::Xml Qt5::Widgets Qt5::Xml KF5::I18n soundkonvertercore)

kservice_desktop_to_json(soundkonverter_filter_sox soundkonverter_filter_sox.desktop)

# install(TARGETS soundkonverter_filter_sox DESTINATION ${PLUGIN_INSTALL_DIR})
# install(FILES soundkonverter_filter_sox.desktop DESTINATION ${SERVICES_INSTALL_DIR})
