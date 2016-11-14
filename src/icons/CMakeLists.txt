if (KF5_BUILD)
ecm_install_icons(ICONS
    16-apps-soundkonverter.png
    22-apps-soundkonverter.png
    32-apps-soundkonverter.png
    48-apps-soundkonverter.png
    64-apps-soundkonverter.png
    16-apps-soundkonverter-replaygain.png
    22-apps-soundkonverter-replaygain.png
    32-apps-soundkonverter-replaygain.png
    48-apps-soundkonverter-replaygain.png
    64-apps-soundkonverter-replaygain.png
    DESTINATION ${ICON_INSTALL_DIR}
)
else ()
    kde4_install_icons(${ICON_INSTALL_DIR})
endif ()
