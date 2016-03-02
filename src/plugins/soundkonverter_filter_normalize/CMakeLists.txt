project(soundkonverter_filter_normalize)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_filter_normalize_SRCS
   soundkonverter_filter_normalize.cpp
   normalizefilteroptions.cpp
   normalizefilterwidget.cpp
 )

kde4_add_plugin(soundkonverter_filter_normalize ${soundkonverter_filter_normalize_SRCS})

target_link_libraries(soundkonverter_filter_normalize ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY} soundkonvertercore )

########### install files ###############

install(TARGETS soundkonverter_filter_normalize DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_filter_normalize.desktop DESTINATION ${SERVICES_INSTALL_DIR})
