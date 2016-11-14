project(soundkonverter_filter_sox)

find_package(KF5 REQUIRED COMPONENTS WidgetsAddons)
find_package(Qt5 REQUIRED COMPONENTS Xml)

set(soundkonverter_filter_sox_SRCS
   soundkonverter_filter_sox.cpp
   soxfilteroptions.cpp
   soxcodecwidget.cpp
   soxfilterwidget.cpp
   soxeffectwidget.cpp
)

add_library(soundkonverter_filter_sox MODULE ${soundkonverter_filter_sox_SRCS})
target_link_libraries(soundkonverter_filter_sox KF5::WidgetsAddons Qt5::Xml soundkonvertercore)

########### install files ###############

install(TARGETS soundkonverter_filter_sox DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_filter_sox.desktop DESTINATION ${SERVICES_INSTALL_DIR})
