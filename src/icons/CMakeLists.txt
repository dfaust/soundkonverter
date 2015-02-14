if (KF5_BUILD)
ecm_install_icons(ICONS hi16-apps-soundkonverter.png
                        hi22-apps-soundkonverter.png
                        hi32-apps-soundkonverter.png
                        hi48-apps-soundkonverter.png
                        hi64-apps-soundkonverter.png
                        hi16-apps-soundkonverter-replaygain.png
                        hi22-apps-soundkonverter-replaygain.png
                        hi32-apps-soundkonverter-replaygain.png
                        hi48-apps-soundkonverter-replaygain.png
                        hi64-apps-soundkonverter-replaygain.png
                        DESTINATION ${ICON_INSTALL_DIR}
)
else ()
    kde4_install_icons(${ICON_INSTALL_DIR})
endif ()
