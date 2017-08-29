project(soundkonverter_replaygain_wvgain)

find_package(KF5 REQUIRED COMPONENTS WidgetsAddons)

set(soundkonverter_replaygain_wvgain_SRCS
   soundkonverter_replaygain_wvgain.cpp
)

add_library(soundkonverter_replaygain_wvgain MODULE ${soundkonverter_replaygain_wvgain_SRCS})
target_link_libraries(soundkonverter_replaygain_wvgain KF5::WidgetsAddons soundkonvertercore)

########### install files ###############

install(TARGETS soundkonverter_replaygain_wvgain DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_replaygain_wvgain.desktop DESTINATION ${SERVICES_INSTALL_DIR})
