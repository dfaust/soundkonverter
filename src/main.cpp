
#include "soundkonverterapp.h"
#include "soundkonverter.h"
#include "global.h"

#include <QApplication>
#include <KAboutData>

#include <KLocalizedString>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>
#include <kcrash.h>

int main(int argc, char **argv)
{
    qDebug() << "main";

    soundKonverterApp app(argc, argv);
//     app.setAttribute(Qt::AA_UseHighDpiPixmaps, true); // Qt 5.4 http://blog.davidedmundson.co.uk/blog/kde_apps_high_dpi

    KCrash::setDrKonqiEnabled(true);

    const QString description = i18n("soundKonverter is a frontend to various audio converters, Replay Gain tools and CD rippers.\n\nPlease file bug reports at https://github.com/HessiJames/soundkonverter/issues\nor send an e-mail to hessijames@gmail.com");

    KAboutData aboutData("soundkonverter", i18n("soundKonverter"), SOUNDKONVERTER_VERSION_STRING);
    aboutData.setShortDescription(description);
    aboutData.setLicense(KAboutLicense::GPL_V2);
    aboutData.setCopyrightStatement(i18n("Copyright (C) 2005-2015 Daniel Faust"));
    aboutData.setHomepage("https://github.com/HessiJames/soundkonverter");
    aboutData.setBugAddress("hessijames@gmail.com");
    aboutData.addAuthor(i18n("Daniel Faust"), "", "hessijames@gmail.com");
    aboutData.addCredit(i18n("David Vignoni"), i18n("Nuvola icon theme"), "", "http://www.icon-king.com");
    aboutData.addCredit(i18n("Scott Wheeler"), i18n("TagLib"), "wheeler@kde.org", "http://ktown.kde.org/~wheeler");
    aboutData.addCredit(i18n("Marco Nelles"), i18n("Audex"), "", "http://opensource.maniatek.de/audex");
    aboutData.addCredit(i18n("Amarok developers"), i18n("Amarok"), "", "http://amarok.kde.org");
    aboutData.addCredit(i18n("All programmers of audio converters"), i18n("Backends"));
    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("replaygain"), i18n("Open the Replay Gain tool and add all given files")));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("rip"), i18n("List all tracks on the cd drive <device>, 'auto' will search for a cd"), QLatin1String("device")));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("profile"), i18n("Add all files using the given profile"), QLatin1String("profile")));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("format"), i18n("Add all files using the given format"), QLatin1String("format")));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("output"), i18n("Output all files to <directory>"), QLatin1String("directory")));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("invisible"), i18n("Start soundKonverter invisible")));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("autostart"), i18n("Start the conversion immediately (enabled when using '--invisible')")));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("autoclose"), i18n("Close soundKonverter after all files are converted (enabled when using '--invisible')")));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("command"), i18n("Execute <command> after each file has been converted (%i=input file, %o=output file)"), QLatin1String("command")));
    parser.addOption(QCommandLineOption(QStringList() << QLatin1String("+[files]"), i18n("Audio file(s) to append to the file list")));

    return app.exec();
}
