#ifndef CONFIGURE_H
#define CONFIGURE_H

#include <QString>
#include <QMap>
#include <QJsonObject>
#include <QJsonValue>
#include <QStandardPaths>

namespace qtuimage
{
    class Configure
    {
        static Configure *singleton;
        const QString filename = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/config.json";
        QJsonObject values;
        Configure();

    public:
        static Configure *getSingleton();
        static void removeSingleton();
        ~Configure();
        template <typename T>
        void set(const QString &key, const T &value)
        {
            values[key] = value;
        }
        template <typename T>
        T get(const QString &key) const;
        template <typename T>
        T get(const QString &key, const T &defaultValue) const;
    };

    template <>
    int Configure::get<int>(const QString &key) const;
    template <>
    QString Configure::get<QString>(const QString &key) const;
    template <>
    int Configure::get<int>(const QString &key, const int &defaultValue) const;
    template <>
    QString Configure::get<QString>(const QString &key, const QString &defaultValue) const;
};

#endif // CONFIGURE_H
