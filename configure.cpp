#include "configure.h"
#include "qjsonobject.h"

#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QTextStream>

using namespace qtuimage;

Configure *Configure::singleton = nullptr;

Configure *Configure::getSingleton()
{
    if (!singleton)
        singleton = new Configure();

    return singleton;
}

void Configure::removeSingleton()
{
    delete singleton;
    singleton = nullptr;
}

Configure::Configure()
{
    qDebug() << filename;
    QFile file(filename);
    if (file.exists())
    {
        file.open(QFile::ReadOnly);
        QTextStream in(&file);

        QJsonDocument doc = QJsonDocument::fromJson(in.readAll().toUtf8());
        values = doc.object();
    }
    else
    {
        QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
        qDebug() << dir.absolutePath();
        if (!dir.exists())
            dir.mkdir(dir.absolutePath());

        values["window.width"] = 1200;
        values["window.height"] = 900;
        values["window.pos_x"] = 60;
        values["window.pos_y"] = 45;
        values["directory"] = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    }
}

Configure::~Configure()
{
    QFile file(filename);
    file.open(QFile::WriteOnly);

    QJsonDocument doc = QJsonDocument(values);
    file.write(doc.toJson());
}

template <>
int Configure::get<int>(const QString &key) const
{
    return values[key].toInt();
}

template <>
QString Configure::get<QString>(const QString &key) const
{
    return values[key].toString();
}

template <>
int Configure::get<int>(const QString &key, const int &defaultValue) const
{
    return values[key].toInt(defaultValue);
}

template <>
QString Configure::get<QString>(const QString &key, const QString &defaultValue) const
{
    return values[key].toString(defaultValue);
}
