#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QObject>
#include <QMap>
#include <QImage>
#include <QSharedPointer>
#include <QStringList>
#include <QMutex>

namespace qtuimage
{
    class ImageLoader : public QObject
    {
        Q_OBJECT
        static ImageLoader *singleton;

        QMap<QString, QSharedPointer<QImage>> images;
        QStringList requested;
        QMutex mtx;

        explicit ImageLoader(QObject *parent = nullptr);
        void loadImage(const QString &path);

    public:
        static ImageLoader *getSingleton();
        bool isLoading(const QString &path);
        void request(const QString &path);

    signals:
        void requestHandled(const QString &path, QSharedPointer<QImage> result);
    };
};

#endif // IMAGELOADER_H
