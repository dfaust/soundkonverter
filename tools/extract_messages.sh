#!/bin/sh
# http://techbase.kde.org/Development/Tutorials/Localization/i18n_Build_Systems
BASEDIR="../src/" # root of translatable sources
PROJECT="soundkonverter" # project name
BUGADDR="https://bugs.launchpad.net/ubuntu/%2Bsource/soundkonverter/" # MSGID-Bugs
WDIR=`pwd` # working dir


echo "Preparing rc files"
cd ${BASEDIR}
# we use simple sorting to make sure the lines do not jump around too much from system to system
find . -name '*.rc' -o -name '*.ui' -o -name '*.kcfg' | sort > ${WDIR}/rcfiles.list
xargs --arg-file=${WDIR}/rcfiles.list extractrc > ${WDIR}/rc.cpp
# additional string for KAboutData
echo 'i18nc("NAME OF TRANSLATORS","Your names");' >> ${WDIR}/rc.cpp
echo 'i18nc("EMAIL OF TRANSLATORS","Your emails");' >> ${WDIR}/rc.cpp
intltool-extract --quiet --type=gettext/ini soundkonverter.desktop.template
intltool-extract --quiet --type=gettext/ini soundkonverter-rip-audiocd.desktop.template
cat *.desktop.template.h >> ${WDIR}/rc.cpp
rm *.desktop.template.h
cd ${WDIR}
echo "Done preparing rc files"


echo "Extracting messages"
cd ${BASEDIR}
# see above on sorting
find . -name '*.cpp' -o -name '*.h' -o -name '*.c' | sort > ${WDIR}/infiles.list
echo "rc.cpp" >> ${WDIR}/infiles.list
cd ${WDIR}
xgettext --from-code=UTF-8 -C -kde -ci18n -kN_:1 -ki18n:1 -ki18nc:1c,2 -ki18np:1,2 -ki18ncp:1c,2,3 -ktr2i18n:1 \
-kI18N_NOOP:1 -kI18N_NOOP2:1c,2 -kaliasLocale -kki18n:1 -kki18nc:1c,2 -kki18np:1,2 -kki18ncp:1c,2,3 \
--msgid-bugs-address="${BUGADDR}" \
--files-from=infiles.list -D ${BASEDIR} -D ${WDIR} -o ${BASEDIR}/po/${PROJECT}.pot || { echo "error while calling xgettext. aborting."; exit 1; }
echo "Done extracting messages"


echo "Merging translations"
cd ${BASEDIR}/po
catalogs=`find . -name '*.po'`
for catalog in $catalogs; do
echo $catalog
msgmerge -o $catalog.new $catalog ${PROJECT}.pot
mv $catalog.new $catalog
done
cd ..
intltool-merge --quiet --desktop-style ./po soundkonverter.desktop.template soundkonverter.desktop
intltool-merge --quiet --desktop-style ./po soundkonverter-rip-audiocd.desktop.template soundkonverter-rip-audiocd.desktop
echo "Done merging translations"


echo "Cleaning up"
cd ${WDIR}
rm rcfiles.list
rm infiles.list
rm rc.cpp
echo "Done"
