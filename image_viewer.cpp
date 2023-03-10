#include "image_viewer.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QDebug>
#include <algorithm>
#include <QDir>
#include <QFileInfo>
#include <QQueue>
#include <QDirIterator>

#include "image_loader.h"
#include "thumbnail_loader.h"
#include "configure.h"

using namespace qtuimage;

ImageViewer::ImageViewer(QWidget *parent)
    : QGraphicsView(parent),
      leftClick(false),
      current(-1),
      thumbnailLoadRequest(0),
      lastLoadedThumbnail(0)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCacheMode(QGraphicsView::CacheBackground);
    setMouseTracking(true);

    QPalette pal(palette());
    pal.setColor(QPalette::Base, Qt::darkGray);
    setAutoFillBackground(true);
    setPalette(pal);

    connect(ImageLoader::getSingleton(), &ImageLoader::requestHandled, this, &ImageViewer::registerImage);
    connect(ThumbnailLoader::getSingleton(), &ThumbnailLoader::requestHandled, this, &ImageViewer::registerThumbnail);
}

bool ImageViewer::isCurrentReady() const
{
    return 0 <= current && imageData[paths[current]] && imageData[paths[current]]->main;
}

bool ImageViewer::isThumbnailReady(const QString& path) const
{
    return imageData[path]->thumbnail!=nullptr;
}

QSize ImageViewer::sizeHint(void) const
{
    paths.begin();
    if (isCurrentReady())
    {
        auto r = imageData[paths[current]]->main->getDisplayRect().size();
        return QSize(r.width(), r.height());
    }
    else
        return QSize(0, 0);
}

void ImageViewer::keyPressEvent(QKeyEvent *event)
{
    const int diff = 32;
    switch (event->key())
    {
    case Qt::Key_Tab:
        current++;
        if (current == paths.size())
            current = 0;
        onThumbnailSelected(paths[current]);
        break;
    case Qt::Key_Backtab:
        current--;
        if (current < 0)
            current = paths.size() - 1;
        onThumbnailSelected(paths[current]);
        break;
    case Qt::Key_Plus:
    case Qt::Key_Semicolon:
        _scaleing(0.1, geometry().center());
        break;
    case Qt::Key_Minus:
    case Qt::Key_hyphen:
        _scaleing(-0.1, geometry().center());
        break;
    case Qt::Key_Left:
        imageData[paths[current]]->main->global.rx() += diff;
        invokeRepaint();
        break;
    case Qt::Key_Right:
        imageData[paths[current]]->main->global.rx() -= diff;
        invokeRepaint();
        break;
    case Qt::Key_Up:
        imageData[paths[current]]->main->global.ry() += diff;
        invokeRepaint();
        break;
    case Qt::Key_Down:
        imageData[paths[current]]->main->global.ry() -= diff;
        invokeRepaint();
        break;
    }
}

void ImageViewer::paintEvent(QPaintEvent *)
{
    if (0 <= current)
    {
        const auto &path = paths[current];
        const auto &main = imageData[path]->main;
        if (main)
        {
            QPainter painter(viewport());
            painter.drawImage(main->getDisplayRect(), *main->image);
        }
        else
            ImageLoader::getSingleton()->request(path);
    }
}

void ImageViewer::invokeRepaint()
{
    adjustImageScale();
    adjustImagePosition();
    viewport()->repaint();
}

void ImageViewer::adjustImageScale()
{
    auto &d = imageData[paths[current]]->main;
    auto is = d->image->size();
    const auto vs = viewport()->size();
    float scale;
    switch (d->scalingMode)
    {
    case ImageXform::ScalingMode::ACTUAL_SIZE:
        scale = 1.0;
        break;
    case ImageXform::ScalingMode::FIT_WINDOW:
        scale = std::min({1.0f, (float)vs.width() / is.width(), (float)vs.height() / is.height()});
        break;
    case ImageXform::ScalingMode::USER_MANIPULATION:
        scale = std::max(std::min({1.0f, (float)vs.width() / is.width(), (float)vs.height() / is.height()}), d->scale);
        break;
    }
    d->scale = scale;

    if (d->scale == 1.0)
        d->scalingMode = ImageXform::ScalingMode::ACTUAL_SIZE;
    else if (scale == (float)vs.width() / is.width() || scale == (float)vs.height() / is.height())
        d->scalingMode = ImageXform::ScalingMode::FIT_WINDOW;
    else
        d->scalingMode = ImageXform::ScalingMode::USER_MANIPULATION;
}

void ImageViewer::adjustImagePosition()
{
    auto &main = imageData[paths[current]]->main;
    auto is = main->image->size();
    auto centerGlobal = main->xform().map(QPointF(is.width(), is.height()) / 2.0);

    auto ds = main->getDisplayRect().size();
    auto vs = viewport()->size();
    if (ds.width() <= vs.width())
        centerGlobal.setX(vs.width() / 2.0);
    if (ds.height() <= vs.height())
        centerGlobal.setY(vs.height() / 2.0);

    main->overlapLocalOnGlobal(QPointF(is.width(), is.height()) / 2.0, centerGlobal);

    if (vs.width() < ds.width())
        main->global.setX(std::clamp(main->global.x(), (double)vs.width() - ds.width(), 0.0));
    if (vs.height() < ds.height())
        main->global.setY(std::clamp(main->global.y(), (double)vs.height() - ds.height(), 0.0));
}

void ImageViewer::mouseMoveEvent(QMouseEvent *event)
{
    if (leftClick)
    {
        if (isCurrentReady())
        {
            auto diff = event->pos() - mousePosition;
            imageData[paths[current]]->main->global += diff;
            invokeRepaint();
        }
    }
    mousePosition = event->pos();
    emit mouseMoved(event);
}

void ImageViewer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        leftClick = true;
}

void ImageViewer::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        leftClick = false;
}

void ImageViewer::resizeEvent(QResizeEvent *)
{
    if (isCurrentReady())
        invokeRepaint();
}

void ImageViewer::_scaleing(float s, const QPointF &center)
{
    auto &main = imageData[paths[current]]->main;
    auto lp = main->xform().inverted().map(center);
    main->scalingMode = ImageXform::ScalingMode::USER_MANIPULATION;
    main->setLog10Scale(main->getLog10Scale() + s);
    adjustImageScale();
    main->overlapLocalOnGlobal(lp, center);
    invokeRepaint();
}

void ImageViewer::wheelEvent(QWheelEvent *event)
{
    if (isCurrentReady())
    {
        const double steps = event->angleDelta().y() / 120.0;
        _scaleing(steps * 0.1, event->position());
    }
}

void ImageViewer::addPaths(const QString &path)
{
    auto info = QFileInfo(path);
    auto *cfg = Configure::getSingleton();
    if (!info.exists())
        return;
    if (info.isFile())
    {
        cfg->set("directory", info.path());
        for (const auto &ext : supportedExtensions)
        {
            if (path.endsWith(ext))
            {
                addImage(path);
                return;
            }
        }
    }
    else if (info.isDir())
    {
        cfg->set("directory", path);
        for (const auto &child : QDir(path).entryInfoList(QDir::Files))
        {
            for (const auto &ext : supportedExtensions)
            {
                if (child.filePath().endsWith(ext))
                {
                    addImage(child.filePath());
                    break;
                }
            }
        }
    }
}

void ImageViewer::addPathsRecursive(const QString &path)
{
    auto info = QFileInfo(path);
    if (!info.exists())
        return;
    auto *cfg = Configure::getSingleton();
    if (info.isFile())
    {
        cfg->set("directory", info.path());
        for (const auto &ext : supportedExtensions)
        {
            if (path.endsWith(ext))
            {
                addImage(path);
                return;
            }
        }
    }
    else if (info.isDir())
    {
        cfg->set("directory", path);
        QDirIterator directories(path, QDir::Files, QDirIterator::Subdirectories);
        while (directories.hasNext())
        {
            directories.next();
            for (const auto &ext : supportedExtensions)
            {
                if (directories.filePath().endsWith(ext))
                {
                    addImage(directories.filePath());
                    break;
                }
            }
        }
    }
}

void ImageViewer::addImage(const QString &path)
{
    qDebug() << "add image: " << path;
    paths.push_back(path);
    imageData[path] = QSharedPointer<ImageData>(new ImageData);
    if(thumbnailLoadRequest<=maxThumbnailLoadRequest)
    {
        ThumbnailLoader::getSingleton()->request(path);
        thumbnailLoadRequest++;
    }

    if (current < 0)
        current = 0;

    emit pathsChanged();
}

void ImageViewer::registerImage(const QString &path, QSharedPointer<QImage> image)
{
    auto &d = imageData[path];
    d->main = QSharedPointer<ImageXform>(new ImageXform(image));
    if (isCurrentReady())
        invokeRepaint();
}

void ImageViewer::removeImage(const QString &path)
{
    paths.erase(paths.begin() + paths.indexOf(path));
    imageData.erase(imageData.find(path));

    emit pathsChanged();
}

void ImageViewer::removeImage(int index)
{
    removeImage(paths[index]);
}

void ImageViewer::registerThumbnail(const QString &path, QSharedPointer<QPixmap> pixmap)
{
    if(0<thumbnailLoadRequest)
        thumbnailLoadRequest--;

    auto &d = imageData[path];
    d->thumbnail = pixmap;

    emit thumbnailRegistered(path, pixmap);
}

void ImageViewer::onThumbnailSelected(const QString &path)
{
    current = paths.indexOf(path);

    if (isCurrentReady())
        invokeRepaint();
    else
        ImageLoader::getSingleton()->request(path);
}

void ImageViewer::onThumbnailScrolled(int left)
{
    for(int i=lastLoadedThumbnail;i<left+maxThumbnailLoadRequest;i++)
    {
        if(paths.size()<=i)
            break;
        if(!isThumbnailReady(paths[i]))
            ThumbnailLoader::getSingleton()->request(paths[i]);
        lastLoadedThumbnail=i;
    }
}
