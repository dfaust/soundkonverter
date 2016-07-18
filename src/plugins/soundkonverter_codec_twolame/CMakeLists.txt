project(soundkonverter_codec_twolame)

if (KF5_BUILD)
    find_package(KF5 REQUIRED COMPONENTS WidgetsAddons)
    find_package(Qt5 REQUIRED COMPONENTS Xml)
else ()
    find_package(KDE4 REQUIRED)
    include (KDE4Defaults)
    include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )
endif ()

set(soundkonverter_codec_twolame_SRCS
   soundkonverter_codec_twolame.cpp
   twolamecodecwidget.cpp
 )

if (KF5_BUILD)
    add_library(soundkonverter_codec_twolame MODULE ${soundkonverter_codec_twolame_SRCS})
    target_link_libraries(soundkonverter_codec_twolame KF5::WidgetsAddons Qt5::Xml soundkonvertercore )
else ()
    kde4_add_plugin(soundkonverter_codec_twolame ${soundkonverter_codec_twolame_SRCS})
    target_link_libraries(soundkonverter_codec_twolame ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY} soundkonvertercore )
endif ()

########### install files ###############

install(TARGETS soundkonverter_codec_twolame DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_twolame.desktop DESTINATION ${SERVICES_INSTALL_DIR})
