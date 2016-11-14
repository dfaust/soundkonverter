project(soundkonverter_replaygain_musepackgain)

find_package(KF5 REQUIRED COMPONENTS WidgetsAddons)

set(soundkonverter_replaygain_musepackgain_SRCS
   soundkonverter_replaygain_musepackgain.cpp
)

add_library(soundkonverter_replaygain_musepackgain MODULE ${soundkonverter_replaygain_musepackgain_SRCS})
target_link_libraries(soundkonverter_replaygain_musepackgain KF5::WidgetsAddons soundkonvertercore)

########### install files ###############

install(TARGETS soundkonverter_replaygain_musepackgain DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES soundkonverter_replaygain_musepackgain.desktop DESTINATION ${SERVICES_INSTALL_DIR})
