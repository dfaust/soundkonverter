project(soundkonverter_codec_flac)

if (KF5_BUILD)
    find_package(KF5 REQUIRED COMPONENTS WidgetsAddons)
    find_package(Qt5 REQUIRED COMPONENTS Xml)
else ()
    find_package(KDE4 REQUIRED)
    include (KDE4Defaults)
    include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )
endif ()

set(soundkonverter_codec_flac_SRCS
   soundkonverter_codec_flac.cpp
   flaccodecwidget.cpp
 )

if (KF5_BUILD)
    add_library(soundkonverter_codec_flac ${soundkonverter_codec_flac_SRCS})
    target_link_libraries(soundkonverter_codec_flac KF5::WidgetsAddons Qt5::Xml soundkonvertercore )
else ()
    kde4_add_plugin(soundkonverter_codec_flac ${soundkonverter_codec_flac_SRCS})
    target_link_libraries(soundkonverter_codec_flac ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY} soundkonvertercore )
endif ()

########### install files ###############

install(TARGETS soundkonverter_codec_flac DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_flac.desktop DESTINATION ${SERVICES_INSTALL_DIR})
