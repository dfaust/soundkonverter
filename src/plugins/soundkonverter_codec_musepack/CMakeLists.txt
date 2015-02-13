project(soundkonverter_codec_musepack)

if (KF5_BUILD)
    find_package(KF5 REQUIRED COMPONENTS WidgetsAddons)
    find_package(Qt5 REQUIRED COMPONENTS Xml)
else ()
    find_package(KDE4 REQUIRED)
    include (KDE4Defaults)
    include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )
endif ()

set(soundkonverter_codec_musepack_SRCS
   soundkonverter_codec_musepack.cpp
   musepackcodecwidget.cpp
   musepackconversionoptions.cpp
 )

if (KF5_BUILD)
    add_library(soundkonverter_codec_musepack ${soundkonverter_codec_musepack_SRCS})
    target_link_libraries(soundkonverter_codec_musepack KF5::WidgetsAddons Qt5::Xml soundkonvertercore )
else ()
    kde4_add_plugin(soundkonverter_codec_musepack ${soundkonverter_codec_musepack_SRCS})
    target_link_libraries(soundkonverter_codec_musepack ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY} soundkonvertercore )
endif ()

########### install files ###############

install(TARGETS soundkonverter_codec_musepack DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_musepack.desktop DESTINATION ${SERVICES_INSTALL_DIR})
