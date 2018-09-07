project(soundkonverter)
cmake_minimum_required(VERSION 3.3.0)

set(CMAKE_CXX_STANDARD 11)

# show more warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
# strict ISO C and ISO C++
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
# use c++11 standard
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# implicit conversions
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion")
# implicit conversions from signed to unsigned
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-conversion")

# hide deprecated declarations warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")

# kde libs
find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

add_definitions(-DSOUNDKONVERTER_KF5_BUILD)

include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)
include(CMakePackageConfigHelpers)
include(ECMInstallIcons)
include(FeatureSummary)

find_package(Qt5 REQUIRED COMPONENTS Network Core Widgets)
find_package(KF5 REQUIRED COMPONENTS I18n XmlGui KIO Solid Config ConfigWidgets WidgetsAddons KDELibs4Support)
find_package(KF5Cddb)
find_package(Phonon4Qt5 REQUIRED)


# additional libs
find_package(Cdparanoia REQUIRED)

set(TAGLIB_MIN_VERSION "1.10")
find_package(Taglib REQUIRED)


# plugins 
add_subdirectory(plugins)


# include directories
include_directories(${TAGLIB_INCLUDES} ${CDPARANOIA_INCLUDE_DIR})


# translation
include(ECMOptionalAddSubdirectory)
ecm_optional_add_subdirectory( po )


# soundkonvertercore
set(soundkonvertercore_SRCS
   core/backendplugin.cpp
   core/conversionoptions.cpp
   core/codecwidget.cpp
   core/codecplugin.cpp
   core/filterplugin.cpp
   core/replaygainplugin.cpp
   core/ripperplugin.cpp
)

add_library(soundkonvertercore SHARED ${soundkonvertercore_SRCS})
target_link_libraries(soundkonvertercore KF5::WidgetsAddons KF5::KIOFileWidgets KF5::KIOCore KF5::KDELibs4Support)

install(TARGETS soundkonvertercore DESTINATION ${LIB_INSTALL_DIR})


# soundkonverter
set(soundkonverter_SRCS
   configdialog/configdialog.cpp
   configdialog/configpagebase.cpp
   configdialog/configgeneralpage.cpp
   configdialog/configbackendspage.cpp
   configdialog/configadvancedpage.cpp
   configdialog/configcoverartpage.cpp
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
   filelist.cpp
   filelistitem.cpp
   logger.cpp
   logviewer.cpp
   options.cpp
   optionssimple.cpp
   optionsdetailed.cpp
   optionseditor.cpp
   optionslayer.cpp
   pluginloader.cpp
   progressindicator.cpp
   outputdirectory.cpp
   aboutplugins.cpp
)

add_executable(soundkonverter ${soundkonverter_SRCS})
target_link_libraries(soundkonverter KF5::WidgetsAddons KF5::KIOFileWidgets KF5::KIOCore KF5::Solid KF5::Cddb Phonon::phonon4qt5 ${TAGLIB_LIBRARIES} ${CDPARANOIA_LIBRARIES} KF5::KDELibs4Support soundkonvertercore)

install(TARGETS soundkonverter DESTINATION ${BIN_INSTALL_DIR})


# install directories
install(FILES soundkonverter.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES soundkonverterui.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/soundkonverter)

install(FILES soundkonverter_codecplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
install(FILES soundkonverter_filterplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
install(FILES soundkonverter_replaygainplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
install(FILES soundkonverter_ripperplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
install(FILES soundkonverter-rip-audiocd.desktop DESTINATION ${DATA_INSTALL_DIR}/solid/actions)

install(FILES soundkonverter.appdata.xml DESTINATION ${SHARE_INSTALL_PREFIX}/metainfo)

add_subdirectory(icons)
add_subdirectory(images)


# summary
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
