project(soundkonverter_filter_sox)

if (KF5_BUILD)
    find_package(KF5 REQUIRED COMPONENTS WidgetsAddons)
    find_package(Qt5 REQUIRED COMPONENTS Xml)
else ()
    find_package(KDE4 REQUIRED)
    include (KDE4Defaults)
    include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )
endif ()

set(soundkonverter_filter_sox_SRCS
   soundkonverter_filter_sox.cpp
   soxfilteroptions.cpp
   soxcodecwidget.cpp
   soxfilterwidget.cpp
   soxeffectwidget.cpp
 )

if (KF5_BUILD)
    add_library(soundkonverter_filter_sox MODULE ${soundkonverter_filter_sox_SRCS})
    target_link_libraries(soundkonverter_filter_sox KF5::WidgetsAddons Qt5::Xml soundkonvertercore )
else ()
    kde4_add_plugin(soundkonverter_filter_sox ${soundkonverter_filter_sox_SRCS})
    target_link_libraries(soundkonverter_filter_sox ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY} soundkonvertercore )
endif ()

########### install files ###############

install(TARGETS soundkonverter_filter_sox DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_filter_sox.desktop DESTINATION ${SERVICES_INSTALL_DIR})
