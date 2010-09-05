project(soundkonverter_codec_wavpack)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_codec_wavpack_SRCS
   soundkonverter_codec_wavpack.cpp
   wavpackcodecwidget.cpp
 )

kde4_add_plugin(soundkonverter_codec_wavpack ${soundkonverter_codec_wavpack_SRCS})

# kde4_add_library(soundkonverter_codec_wavpack SHARED ${soundkonverter_codec_wavpack_SRCS})

target_link_libraries(soundkonverter_codec_wavpack ${KDE4_KDEUI_LIBS} ${QT_QTXML_LIBRARY} soundkonvertercore )
#target_link_libraries(soundkonverter_codec_wavpack ${KDE4_KIO_LIBS} )

########### install files ###############

install(TARGETS soundkonverter_codec_wavpack DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_codec_wavpack.desktop DESTINATION ${SERVICES_INSTALL_DIR})
