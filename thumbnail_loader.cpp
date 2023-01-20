#include "thumbnail_loader.h"
#include <QtConcurrent>
#include <QDebug>
#include <QCryptographicHash>
#include <QImageReader>

using namespace qtuimage;

ThumbnailLoader *ThumbnailLoader::singleton = nullptr;
const QString ThumbnailLoader::thumbnailCachePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/qtuimage/thumbnails/";

ThumbnailLoader::ThumbnailLoader(QObject *parent)
    : QObject(parent)
{
    QDir dir;
    if (!dir.exists(thumbnailCachePath))
        dir.mkpath(thumbnailCachePath);
}

ThumbnailLoader *ThumbnailLoader::getSingleton()
{
    if (!singleton)
        singleton = new ThumbnailLoader;

    return singleton;
}

QString ThumbnailLoader::getHashString(const QString &str)
{
    return QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Sha256).toHex();
}

void ThumbnailLoader::loadPixmap(const QString &path)
{
    QImageReader reader(path);
    reader.setAllocationLimit(0);

    auto original = QPixmap::fromImageReader(&reader);
    QSharedPointer<QPixmap> p;
    QString cachePath = thumbnailCachePath + getHashString(path) + ".jpg";
    if (QDir().exists(cachePath))
    {
        p.reset(new QPixmap(cachePath));
    }
    else
    {
        p.reset(new QPixmap);
        *p = original.scaled(QSize(thumbnailSize, thumbnailSize), Qt::KeepAspectRatio);
        if (p.isNull())
            qDebug() << "failed to load thumbnail: " << path;
        else
            qDebug() << "load thumbnail: " << path << " (" << p->size() << ")";
        p->save(cachePath);
    }

    emit requestHandled(path, p);
}

void ThumbnailLoader::request(const QString &path)
{
    auto ret = QtConcurrent::task([&](const QString &p)
                                  { loadPixmap(p); })
                   .withArguments(path)
                   .spawn();
}
