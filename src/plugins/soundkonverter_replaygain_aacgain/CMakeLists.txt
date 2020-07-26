project(soundkonverter_replaygain_aacgain)

set(soundkonverter_replaygain_aacgain_SRCS
   soundkonverter_replaygain_aacgain.cpp
)

add_library(soundkonverter_replaygain_aacgain MODULE ${soundkonverter_replaygain_aacgain_SRCS})
target_link_libraries(soundkonverter_replaygain_aacgain KF5::WidgetsAddons soundkonvertercore)

########### install files ###############

install(TARGETS soundkonverter_replaygain_aacgain DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_replaygain_aacgain.desktop DESTINATION ${SERVICES_INSTALL_DIR})
