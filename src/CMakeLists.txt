project(soundkonverter)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)

# find_package(Cdparanoia)
#macro_log_feature(CDPARANOIA_FOUND "cdparanoia" "Reads digital audio directly from a CD, then writes the data to a file or pipe in several different formats" "http://www.xiph.org/paranoia/" FALSE "" "Required for building Audex.")

set(TAGLIB_MIN_VERSION "1.4")
find_package(Taglib REQUIRED)

# # Check if TagLib is built with ASF and MP4 support
# include(CheckCXXSourceCompiles)
# set(CMAKE_REQUIRED_INCLUDES "${TAGLIB_INCLUDES}")
# set(CMAKE_REQUIRED_LIBRARIES "${TAGLIB_LIBRARIES}")
# 
# check_cxx_source_compiles("#include <asftag.h>
# int main() { TagLib::ASF::Tag tag; return 0;}" TAGLIB_ASF_FOUND)
# if( NOT TAGLIB_ASF_FOUND )
#     message(FATAL_ERROR "TagLib does not have ASF support compiled in.")
# endif( NOT TAGLIB_ASF_FOUND )
# 
# check_cxx_source_compiles("#include <mp4tag.h>
# int main() { TagLib::MP4::Tag tag(0, 0); return 0;}" TAGLIB_MP4_FOUND)
# if( NOT TAGLIB_MP4_FOUND )
#     message(FATAL_ERROR "TagLib does not have MP4 support compiled in.")
# endif( NOT TAGLIB_MP4_FOUND )
# 
# set(CMAKE_REQUIRED_INCLUDES)
# set(CMAKE_REQUIRED_LIBRARIES)
# 
# set(TAGLIB-EXTRAS_MIN_VERSION "1.0")
# find_package(Taglib-Extras REQUIRED)
# 
# include(CheckTagLibFileName)
# 
# check_taglib_filename(COMPLEX_TAGLIB_FILENAME)
# 
#


include_directories(${KDE4_INCLUDES} ${QT_INCLUDES} ${TAGLIB_INCLUDES})

add_subdirectory(plugins/soundkonverter_codec_lame)
add_subdirectory(plugins/soundkonverter_codec_vorbistools)
add_subdirectory(plugins/soundkonverter_codec_flac)
add_subdirectory(plugins/soundkonverter_codec_ffmpeg)
add_subdirectory(plugins/soundkonverter_codec_mplayer)
add_subdirectory(plugins/soundkonverter_codec_faac)
add_subdirectory(plugins/soundkonverter_codec_neroaac)
add_subdirectory(plugins/soundkonverter_codec_timidity)
add_subdirectory(plugins/soundkonverter_replaygain_vorbisgain)
add_subdirectory(plugins/soundkonverter_replaygain_metaflac)
add_subdirectory(plugins/soundkonverter_replaygain_mp3gain)
add_subdirectory(plugins/soundkonverter_replaygain_aacgain)
add_subdirectory(plugins/soundkonverter_ripper_cdparanoia)

include(MacroOptionalAddSubdirectory)
macro_optional_add_subdirectory( po )


set(soundkonvertercore_SRCS
   core/backendplugin.cpp
   core/codecplugin.cpp
   core/codecwidget.cpp
   core/conversionoptions.cpp
   core/replaygainplugin.cpp
   core/ripperplugin.cpp
)

kde4_add_library(soundkonvertercore SHARED ${soundkonvertercore_SRCS})

target_link_libraries(soundkonvertercore ${KDE4_KDEUI_LIBS} ${KDE4_KFILE_LIBS} ${KDE4_KIO_LIBS})
#set_target_properties(soundkonvertercore PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} )
install(TARGETS soundkonvertercore DESTINATION ${LIB_INSTALL_DIR})


# add_definitions(${TAGLIB_CFLAGS} "${TAGLIB_CFLAGS}/..")
# ADD_DEFINITIONS(${TAGLIB_CFLAGS} -DQT_STL)

set(soundkonverter_SRCS
#    audiocd/cdmanager.cpp
#    audiocd/paranoia.cpp
   configdialog/configdialog.cpp
   configdialog/configpagebase.cpp
   configdialog/configgeneralpage.cpp
   configdialog/configadvancedpage.cpp
   configdialog/configbackendspage.cpp
   metadata/tagengine.cpp
   opener/fileopener.cpp
   opener/diropener.cpp
   opener/cdopener.cpp
   opener/urlopener.cpp
   opener/playlistopener.cpp
   replaygainscanner/replaygainscanner.cpp
   replaygainscanner/replaygainfilelist.cpp
   replaygainscanner/replaygainfilelistitem.cpp
   main.cpp
   soundkonverter.cpp
   soundkonverterapp.cpp
   soundkonverterview.cpp
   config.cpp
   filelist.cpp
   filelistitem.cpp
   combobutton.cpp
   progressindicator.cpp
   options.cpp
   optionssimple.cpp
   optionsdetailed.cpp
   optionslayer.cpp
   optionseditor.cpp
   pluginloader.cpp
   outputdirectory.cpp
   convert.cpp
   logger.cpp
   logviewer.cpp
   aboutplugins.cpp
   conversionoptionsmanager.cpp
   global.cpp
   codecproblems.cpp
 )

#kde4_automoc(${soundkonverter_SRCS})

#kde4_add_ui_files(soundkonverter_SRCS soundkonverterview_base.ui prefs_base.ui)

#kde4_add_kcfg_files(soundkonverter_SRCS settings.kcfgc )

kde4_add_executable(soundkonverter ${soundkonverter_SRCS})

target_link_libraries(soundkonverter ${KDE4_KDEUI_LIBS} ${KDE4_KFILE_LIBS} ${KDE4_KIO_LIBS} ${KDE4_SOLID_LIBS} ${TAGLIB_LIBRARIES} kcddb kcompactdisc soundkonvertercore)

install(TARGETS soundkonverter DESTINATION ${BIN_INSTALL_DIR})


########### install files ###############

install(FILES soundkonverter.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
#install( FILES soundkonverter.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install(FILES soundkonverterui.rc DESTINATION ${DATA_INSTALL_DIR}/soundkonverter)

install(FILES soundkonverter_codecplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
install(FILES soundkonverter_replaygainplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
install(FILES soundkonverter_ripperplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})

add_subdirectory(icons)
add_subdirectory(images)

# dependencies: taglib, cdparanoia
### dependencies: taglib, libmediainfo, zenlib, cdparanoia

macro_display_feature_log()
