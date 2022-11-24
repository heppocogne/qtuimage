#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QObject>
#include <QMap>
#include <QImage>
#include <QStringList>
#include <QMutex>
#include <QAtomicInteger>
#include <QSharedPointer>

namespace qtuimage
{
    class ImageLoader : public QObject
    {
        Q_OBJECT
        static ImageLoader* singleton;

    protected:
        QMap<QString, QSharedPointer<QImage>> images;
        QStringList requested;
        QMutex mtx;
        QAtomicInteger<uint8_t> terminateFlag;

        void loadImage(const QString& path);

    public:
        explicit ImageLoader(QObject *parent = nullptr);

        static ImageLoader* getSingleton();
        void request(const QString& path);

    signals:
        void requestHandled(const QString& path, QSharedPointer<QImage> result);
    };
};

#endif // IMAGELOADER_H
