project(soundkonverter_filter_normalize)

set(soundkonverter_filter_normalize_SRCS
   soundkonverter_filter_normalize.cpp
   normalizefilteroptions.cpp
   normalizefilterwidget.cpp
)

add_library(soundkonverter_filter_normalize MODULE ${soundkonverter_filter_normalize_SRCS})
target_link_libraries(soundkonverter_filter_normalize KF5::WidgetsAddons Qt5::Xml soundkonvertercore)

########### install files ###############

install(TARGETS soundkonverter_filter_normalize DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_filter_normalize.desktop DESTINATION ${SERVICES_INSTALL_DIR})
