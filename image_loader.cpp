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
        qDebug() << "failed to load image: " << path;
    else
        qDebug() << "load image: " << path << " (" << i->size() << ")";

    emit requestHandled(path, i);

    mtx.lock();
    requested.erase(requested.cbegin() + requested.indexOf(path));
    mtx.unlock();
}

bool ImageLoader::isLoading(const QString &path)
{
    mtx.lock();
    bool result = (0 <= requested.indexOf(path));
    mtx.unlock();

    return result;
}

void ImageLoader::request(const QString &path)
{
    if (isLoading(path))
        return;

    mtx.lock();
    requested.push_back(path);
    mtx.unlock();

    auto ret = QtConcurrent::task([&](const QString &p)
                                  { loadImage(p); })
                   .withArguments(path)
                   .spawn();
}
