#include "thumbnail_loader.h"
#include <QtConcurrent>
#include <QDebug>
#include <QCryptographicHash>

using namespace qtuimage;

ThumbnailLoader *ThumbnailLoader::singleton = nullptr;

ThumbnailLoader::ThumbnailLoader(QObject *parent)
    : QObject(parent)
{
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
    auto original = QPixmap(path);
    QSharedPointer<QPixmap> p(new QPixmap);
    *p = original.scaled(QSize(thumbnailSize, thumbnailSize), Qt::KeepAspectRatio);
    if (p.isNull())
        qDebug() << "failed to load thumbnail: " << path;
    else
        qDebug() << "load thumbnail: " << path << " (" << p->size() << ")";

    // save test
    //p->save(getHashString(path) + ".jpg", "jpg", -1);
    //p->save(getHashString(path) + ".png", "png", 0);

    emit requestHandled(path, p);
}

void ThumbnailLoader::request(const QString &path)
{
    auto ret = QtConcurrent::task([&](const QString &p)
                                  { loadPixmap(p); })
                   .withArguments(path)
                   .spawn();
}
