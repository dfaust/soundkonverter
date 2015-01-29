cmake_minimum_required(VERSION 3.0)
project(soundkonverter)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
# include(FeatureSummary)

find_package(Qt5Widgets)
find_package(Qt5Xml)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(KF5I18n)
find_package(KF5Config)
find_package(KF5XmlGui)
find_package(KF5Notifications)
# find_package(KF5CoreAddons)
# find_package(KF5WidgetsAddons)
find_package(KF5KIO)
find_package(KF5Solid)
find_package(KF5Crash)

find_package(Phonon4Qt5)

find_package(Cdparanoia REQUIRED)

find_package(Discid REQUIRED)
find_package(Musicbrainz5 REQUIRED)

set(TAGLIB_MIN_VERSION "1.9")
find_package(Taglib REQUIRED)

include_directories(${TAGLIB_INCLUDES} ${CDPARANOIA_INCLUDE_DIR})


# all the warnings about constructions
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
# strict ISO C and ISO C++
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
# implicit conversions
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion")
# implicit conversions from signed to unsigned
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-conversion")
# enable exception handling
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")

add_definitions(-DQT_NO_URL_CAST_FROM_STRING)

add_subdirectory(plugins)

# include(MacroOptionalAddSubdirectory)
# ecm_optional_add_subdirectory( po )


set(soundkonvertercore_SRCS
   core/backendplugin.cpp
   core/conversionoptions.cpp
   core/codecwidget.cpp
   core/codecplugin.cpp
   core/filterplugin.cpp
   core/replaygainplugin.cpp
   core/ripperplugin.cpp
)
add_library(soundkonvertercore STATIC ${soundkonvertercore_SRCS})
target_link_libraries(soundkonvertercore Qt5::Widgets Qt5::Xml KF5::I18n)
# install(TARGETS soundkonvertercore DESTINATION ${LIB_INSTALL_DIR})

set(soundkonverter_SRCS
   configdialog/configdialog.cpp
   configdialog/configpagebase.cpp
   configdialog/configgeneralpage.cpp
   configdialog/configbackendspage.cpp
   configdialog/configadvancedpage.cpp
   configdialog/configcoverartpage.cpp
   backendsgroupbox.cpp
   metadata/tagengine.cpp
   metadata/MetaReplayGain.cpp
   opener/fileopener.cpp
   opener/diropener.cpp
   opener/cdopener.cpp
   opener/urlopener.cpp
   opener/playlistopener.cpp
   replaygainscanner/replaygainscanner.cpp
   replaygainscanner/replaygainfilelist.cpp
   replaygainscanner/replaygainfilelistitem.cpp
   replaygainscanner/replaygainprocessor.cpp
   global.cpp
   main.cpp
   soundkonverter.cpp
   soundkonverterapp.cpp
   soundkonverterview.cpp
   codecproblems.cpp
   codecoptimizations.cpp
   combobutton.cpp
   config.cpp
   conversionoptionsmanager.cpp
   convert.cpp
   convertitem.cpp
   directorycrawler.cpp
   filelist.cpp
   filelistitem.cpp
   layer.cpp
   logger.cpp
   logviewer.cpp
   options.cpp
   optionssimple.cpp
   optionsdetailed.cpp
   optionseditor.cpp
   optionslayer.cpp
   progresslayer.cpp
   pluginloader.cpp
   progressindicator.cpp
   outputdirectory.cpp
   aboutplugins.cpp
)

ki18n_wrap_ui(soundkonverter_SRCS
    logviewer.ui
    aboutplugins.ui
    codecproblems.ui
    options.ui
    optionssimple.ui
    optionsdetailed.ui
    outputdirectory.ui
    optionsdialog.ui
    optionslayer.ui
    progresslayer.ui
    soundkonverterview.ui
    configdialog/configgeneralpage.ui
    configdialog/configadvancedpage.ui
    configdialog/configcoverartpage.ui
    configdialog/configbackendspage.ui
    backendsgroupbox.ui
    opener/diropener.ui
    opener/urlopener.ui
    opener/cdopener.ui
    replaygainscanner/replaygainscanner.ui
)

add_executable(soundkonverter ${soundkonverter_SRCS})
target_link_libraries(soundkonverter Qt5::Widgets Qt5::Xml KF5::XmlGui KF5::Notifications KF5::I18n KF5::Solid KF5::KIOFileWidgets KF5::Crash Phonon::phonon4qt5 ${TAGLIB_LIBRARIES} ${DISCID_LIBRARIES} ${MUSICBRAINZ5_LIBRARIES} ${CDPARANOIA_LIBRARIES} soundkonvertercore)
# target_link_libraries(soundkonverter Qt5::Widgets Qt5::Xml KF5::XmlGui KF5::Notifications KF5::I18n KF5::Solid KF5::CoreAddons KF5::WidgetsAddons KF5::ConfigCore KF5::ConfigGui KF5::KIOCore KF5::KIOFileWidgets KF5::KIOWidgets KF5::KIONTLM KF5::Crash Phonon::phonon4qt5 ${TAGLIB_LIBRARIES} ${DISCID_LIBRARIES} ${MUSICBRAINZ5_LIBRARIES} ${CDPARANOIA_LIBRARIES} soundkonvertercore)
# install(TARGETS soundkonverter DESTINATION ${BIN_INSTALL_DIR})

# install(FILES soundkonverter.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
# install(FILES soundkonverterui.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/soundkonverter)

# install(FILES soundkonverter_codecplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
# install(FILES soundkonverter_filterplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
# install(FILES soundkonverter_replaygainplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
# install(FILES soundkonverter_ripperplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
# install(FILES soundkonverter-rip-audiocd.desktop DESTINATION ${DATA_INSTALL_DIR}/solid/actions)
#
# install(FILES soundkonverter.appdata.xml DESTINATION ${SHARE_INSTALL_PREFIX}/appdata)
#
# # add_subdirectory(icons)
# add_subdirectory(images)
#
#
# macro_display_feature_log()
#
#
# # uninstall target
# configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
# add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
