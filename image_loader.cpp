#include "image_loader.h"
#include <QtConcurrent>
#include <QDebug>

using namespace qtuimage;

ImageLoader *ImageLoader::singleton = nullptr;

ImageLoader::ImageLoader(QObject *parent)
    : QObject(parent)
{
}

ImageLoader *ImageLoader::getSingleton()
{
    if (!singleton)
        singleton = new ImageLoader;

    return singleton;
}

void ImageLoader::loadImage(const QString &path)
{
    QSharedPointer<QImage> i(new QImage(path));
    if (i.isNull())
        qDebug() << "load failed: " << path;
    else
        qDebug() << "load: " << path << " (" << i->size() << ")";
    emit requestHandled(path, i);
}

void ImageLoader::request(const QString &path)
{
    qDebug() << "requested: " << path;
    auto ret = QtConcurrent::task([&](const QString &p)
                                  { loadImage(p); })
                   .withArguments(path)
                   .spawn();
}
