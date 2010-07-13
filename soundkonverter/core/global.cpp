
#include <QStringList>

struct FormatInfo {
//     PluginItem::Codec *codec;
    QString codecName;
    bool lossless;
    QString description;
    QStringList mimeTypes;
    QStringList extensions;
};
