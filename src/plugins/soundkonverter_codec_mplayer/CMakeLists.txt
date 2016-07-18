project(soundkonverter_codec_mplayer)

if (KF5_BUILD)
    find_package(KF5 REQUIRED COMPONENTS WidgetsAddons)
    find_package(Qt5 REQUIRED COMPONENTS Xml)
else ()
    find_package(KDE4 REQUIRED)
    include (KDE4Defaults)
    include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )
endif ()

set(soundkonverter_codec_mplayer_SRCS
   soundkonverter_codec_mplayer.cpp
 )

if (KF5_BUILD)
    add_library(soundkonverter_codec_mplayer MODULE ${soundkonverter_codec_mplayer_SRCS})
    target_link_libraries(soundkonverter_codec_mplayer KF5::WidgetsAddons Qt5::Xml soundkonvertercore )
else ()
    kde4_add_plugin(soundkonverter_codec_mplayer ${soundkonverter_codec_mplayer_SRCS})
    target_link_libraries(soundkonverter_codec_mplayer ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY} soundkonvertercore )
endif ()

########### install files ###############

install(TARGETS soundkonverter_codec_mplayer DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_mplayer.desktop DESTINATION ${SERVICES_INSTALL_DIR})
