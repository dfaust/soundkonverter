project(soundkonverter_filter_sox)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_filter_sox_SRCS
   soundkonverter_filter_sox.cpp
   soxfilteroptions.cpp
   soxcodecwidget.cpp
   soxfilterwidget.cpp
   soxeffectwidget.cpp
 )

kde4_add_plugin(soundkonverter_filter_sox ${soundkonverter_filter_sox_SRCS})

target_link_libraries(soundkonverter_filter_sox ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY} soundkonvertercore )

########### install files ###############

install(TARGETS soundkonverter_filter_sox DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_filter_sox.desktop DESTINATION ${SERVICES_INSTALL_DIR})
