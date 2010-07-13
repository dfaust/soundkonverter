project(soundkonverter_replaygain_metaflac)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} )

set(soundkonverter_replaygain_metaflac_SRCS
   soundkonverter_replaygain_metaflac.cpp
 )

kde4_add_plugin(soundkonverter_replaygain_metaflac ${soundkonverter_replaygain_metaflac_SRCS})

# kde4_add_library(soundkonverter_replaygain_metaflac SHARED ${soundkonverter_replaygain_metaflac_SRCS})

target_link_libraries(soundkonverter_replaygain_metaflac ${KDE4_KDEUI_LIBS} soundkonvertercore )
#target_link_libraries(soundkonverter_replaygain_metaflac ${KDE4_KIO_LIBS} )

########### install files ###############

install(TARGETS soundkonverter_replaygain_metaflac DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_replaygain_metaflac.desktop DESTINATION ${SERVICES_INSTALL_DIR})
