#ifndef THUMBNAILLOADER_H
#define THUMBNAILLOADER_H

#include <QObject>
#include <QMap>
#include <QPixmap>
#include <QStringList>
#include <QMutex>
#include <QAtomicInteger>
#include <QSharedPointer>

namespace qtuimage
{
    class ThumbnailLoader : public QObject
    {
        Q_OBJECT
        static ThumbnailLoader *singleton;
        static const QString thumbnailCachePath;

        QMap<QString, QSharedPointer<QPixmap>> pixmaps;

        explicit ThumbnailLoader(QObject *parent = nullptr);
        void loadPixmap(const QString &path);
        static QString getHashString(const QString &str);

    public:
        static const constexpr int thumbnailSize = 64;

        static ThumbnailLoader *getSingleton();
        void request(const QString &path);

    signals:
        void requestHandled(const QString &path, QSharedPointer<QPixmap> result);
    };
};

#endif // IMAGELOADER_H
