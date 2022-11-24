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

#include "image_loader.h"

using namespace qtuimage;

ImageViewer::ImageViewer(QWidget *parent)
    : QGraphicsView(parent),
      leftClick(false),
      current(-1)
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
}

bool ImageViewer::isCurrentReady() const
{
    return 0 <= current && imageData[paths[current]] && imageData[paths[current]]->main;
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
}

void ImageViewer::keyReleaseEvent(QKeyEvent *event)
{
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

    if(d->scale==1.0)
        d->scalingMode=ImageXform::ScalingMode::ACTUAL_SIZE;
    else if(scale==(float)vs.width() / is.width() || scale==(float)vs.height() / is.height())
        d->scalingMode=ImageXform::ScalingMode::FIT_WINDOW;
    else
        d->scalingMode=ImageXform::ScalingMode::USER_MANIPULATION;
}

void ImageViewer::adjustImagePosition()
{
    auto &main = imageData[paths[current]]->main;
    auto is=main->image->size();
    auto centerGlobal=main->xform().map(QPointF(is.width(),is.height())/2.0);

    auto ds=main->getDisplayRect().size();
    auto vs=viewport()->size();
    if(ds.width()<=vs.width())
        centerGlobal.setX(vs.width()/2.0);
    if(ds.height()<=vs.height())
        centerGlobal.setY(vs.height()/2.0);

    main->overlapLocalOnGlobal(QPointF(is.width(),is.height())/2.0,centerGlobal);

    if(vs.width()<ds.width())
        main->global.setX(std::clamp(main->global.x(),(double)vs.width()-ds.width(),0.0));
    if(vs.height()<ds.height())
        main->global.setY(std::clamp(main->global.y(),(double)vs.height()-ds.height(),0.0));
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

void ImageViewer::resizeEvent(QResizeEvent*)
{
    if (isCurrentReady())
        invokeRepaint();
}

void ImageViewer::wheelEvent(QWheelEvent *event)
{
    if (isCurrentReady())
    {
        const double steps = event->angleDelta().y() / 120.0;
        auto &main = imageData[paths[current]]->main;
        auto gp=event->position();
        auto lp=main->xform().inverted().map(gp);
        main->scalingMode=ImageXform::ScalingMode::USER_MANIPULATION;
        main->setLog10Scale(main->getLog10Scale() + 0.1 * steps);
        adjustImageScale();
        main->overlapLocalOnGlobal(lp,gp);
        invokeRepaint();
    }
}

void ImageViewer::addPaths(const QString &path)
{
    auto info=QFileInfo(path);
    if(!info.exists())
        return;
    if(info.isFile())
    {
        for(const auto& ext:supportedExtensions)
        {
            if(path.endsWith(ext))
            {
                addImage(path);
                return;
            }
        }
    }
    else if(info.isDir())
    {
        for(const auto& child:QDir(path).entryInfoList(QDir::Files))
        {
            for(const auto& ext:supportedExtensions)
            {
                if(child.filePath().endsWith(ext))
                    addImage(child.filePath());
            }
        }
    }
}

void ImageViewer::addPathsRecursive(const QString &path)
{
}

void ImageViewer::addImage(const QString &path)
{
    qDebug() << "add image: " << path;
    paths.push_back(path);
    imageData[path] = QSharedPointer<ImageData>(new ImageData);

    if (current < 0)
        current = 0;
}

void ImageViewer::registerImage(const QString &path, QSharedPointer<QImage> image)
{
    qDebug() << "register: " << path << " (" << image->size() << ")";
    auto &d = imageData[path];
    d->main = QSharedPointer<ImageXform>(new ImageXform(image));
    if (isCurrentReady())
        invokeRepaint();
}

void ImageViewer::unregisterImage(const QString &path)
{
    paths.erase(paths.begin() + paths.indexOf(path));
    imageData.erase(imageData.find(path));
}

void ImageViewer::unregisterImage(int index)
{
    unregisterImage(paths[index]);
}
