project(soundkonverter_replaygain_mp3gain)

set(soundkonverter_replaygain_mp3gain_SRCS
   soundkonverter_replaygain_mp3gain.cpp
)

add_library(soundkonverter_replaygain_mp3gain MODULE ${soundkonverter_replaygain_mp3gain_SRCS})
target_link_libraries(soundkonverter_replaygain_mp3gain KF5::WidgetsAddons soundkonvertercore)

########### install files ###############

install(TARGETS soundkonverter_replaygain_mp3gain DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_replaygain_mp3gain.desktop DESTINATION ${SERVICES_INSTALL_DIR})
