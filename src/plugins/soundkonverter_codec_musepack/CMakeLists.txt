project(soundkonverter_codec_musepack)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_codec_musepack_SRCS
   soundkonverter_codec_musepack.cpp
   musepackcodecwidget.cpp
   musepackconversionoptions.cpp
 )

kde4_add_plugin(soundkonverter_codec_musepack ${soundkonverter_codec_musepack_SRCS})

target_link_libraries(soundkonverter_codec_musepack ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY} soundkonvertercore )

########### install files ###############

install(TARGETS soundkonverter_codec_musepack DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_musepack.desktop DESTINATION ${SERVICES_INSTALL_DIR})
