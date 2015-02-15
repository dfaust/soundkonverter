project(soundkonverter)
cmake_minimum_required(VERSION 2.8.12)

cmake_policy(SET CMP0046 NEW)
cmake_policy(SET CMP0002 OLD)


# all the warnings about constructions
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
# strict ISO C and ISO C++
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
# treat warnings as errors
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
# use c++11 standard
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# implicit conversions
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion")
# implicit conversions from signed to unsigned
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-conversion")


# kde libs
if (KF5_BUILD)
    find_package(ECM REQUIRED NO_MODULE)
    set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

    add_definitions(-DSOUNDKONVERTER_KF5_BUILD)

    include(KDEInstallDirs)
    include(KDECompilerSettings)
    include(KDECMakeSettings)
    include(ECMPackageConfigHelpers)
    include(ECMInstallIcons)
    include(FeatureSummary)

    find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
    find_package(KF5 REQUIRED COMPONENTS I18n XmlGui KIO Solid Config ConfigWidgets WidgetsAddons KDELibs4Support)
    find_package(Phonon4Qt5 REQUIRED)
else ()
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

    find_package(KDE4 REQUIRED)
    include(KDE4Defaults)
endif ()


# additional libs
find_package(Cdparanoia REQUIRED)
find_package(Kcddb REQUIRED)

set(TAGLIB_MIN_VERSION "1.4")
find_package(Taglib REQUIRED)


# plugins 
add_subdirectory(plugins)


# include directories
if (KF5_BUILD)
    include_directories(${TAGLIB_INCLUDES} ${CDPARANOIA_INCLUDE_DIR})
else ()
    include_directories(${KDE4_INCLUDES} ${QT_INCLUDES} ${TAGLIB_INCLUDES} ${CDPARANOIA_INCLUDE_DIR})
endif ()


# translation
if (KF5_BUILD)
    include(ECMOptionalAddSubdirectory)
    ecm_optional_add_subdirectory( po )
else ()
    include(MacroOptionalAddSubdirectory)
    macro_optional_add_subdirectory( po )
endif ()


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

if (KF5_BUILD)
    add_library(soundkonvertercore SHARED ${soundkonvertercore_SRCS})
    target_link_libraries(soundkonvertercore KF5::WidgetsAddons KF5::KIOFileWidgets KF5::KIOCore KF5::KDELibs4Support)
else ()
    kde4_add_library(soundkonvertercore SHARED ${soundkonvertercore_SRCS})
    target_link_libraries(soundkonvertercore ${KDE4_KDEUI_LIBS} ${KDE4_KFILE_LIBS} ${KDE4_KIO_LIBS})
endif ()

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

if (KF5_BUILD)
    add_executable(soundkonverter ${soundkonverter_SRCS})
    target_link_libraries(soundkonverter KF5::WidgetsAddons KF5::KIOFileWidgets KF5::KIOCore KF5::Solid Phonon::phonon4qt5 ${TAGLIB_LIBRARIES} kcddb ${CDPARANOIA_LIBRARIES} KF5::KDELibs4Support soundkonvertercore)
else ()
    kde4_add_executable(soundkonverter ${soundkonverter_SRCS})
    target_link_libraries(soundkonverter ${KDE4_KDEUI_LIBS} ${KDE4_KFILE_LIBS} ${KDE4_KIO_LIBS} ${KDE4_SOLID_LIBRARY} ${KDE4_PHONON_LIBS} ${TAGLIB_LIBRARIES} kcddb ${CDPARANOIA_LIBRARIES} soundkonvertercore)
endif ()

install(TARGETS soundkonverter DESTINATION ${BIN_INSTALL_DIR})


# install directories
install(FILES soundkonverter.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
if (KF5_BUILD)
    install(FILES soundkonverterui.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/soundkonverter)
else ()
install(FILES soundkonverterui.rc DESTINATION ${DATA_INSTALL_DIR}/soundkonverter)
endif ()

install(FILES soundkonverter_codecplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
install(FILES soundkonverter_filterplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
install(FILES soundkonverter_replaygainplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
install(FILES soundkonverter_ripperplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
install(FILES soundkonverter-rip-audiocd.desktop DESTINATION ${DATA_INSTALL_DIR}/solid/actions)

install(FILES soundkonverter.appdata.xml DESTINATION ${SHARE_INSTALL_PREFIX}/appdata)

add_subdirectory(icons)
add_subdirectory(images)


# summary
if (KF5_BUILD)
    feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
else ()
    macro_display_feature_log()
endif ()


# uninstall target
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
