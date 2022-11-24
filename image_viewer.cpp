#include "image_viewer.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QDebug>

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

QSize ImageViewer::sizeHint(void) const
{
    paths.begin();
    if (0 <= current && imageData[paths[current]]->main)
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

void ImageViewer::paintEvent(QPaintEvent *event)
{
    if(0<=current)
    {
        const auto& path=paths[current];
        const auto& main=imageData[path]->main;
        if(main)
        {
            QPainter painter(viewport());
            painter.drawImage(main->getDisplayRect(),*main->image);
        }
        else
            ImageLoader::getSingleton()->request(path);
    }
}

void ImageViewer::mouseMoveEvent(QMouseEvent *event)
{
    if (leftClick)
    {
        //qDebug() << "diff=" << event->pos() - mousePosition;
    }
    if (current < 0)
        return;
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

void ImageViewer::resizeEvent(QResizeEvent *event)
{
}

void ImageViewer::wheelEvent(QWheelEvent *event)
{
    const double steps = event->angleDelta().y() / 120.0;
}

void addPath(const QString& path)
{

}

void ImageViewer::addImage(const QString& path)
{
    qDebug()<<"add image: "<<path;
    paths.push_back(path);
    imageData[path]=QSharedPointer<ImageData>(new ImageData);

    if(current<0)
        current=0;
}

void ImageViewer::registerImage(const QString& path, QSharedPointer<QImage> image)
{
    qDebug()<<"register: "<<path<<" ("<<image->size()<<")";
    auto& d=imageData[path];
    d->main=QSharedPointer<ImageXform>(new ImageXform(image));
    if(0<=current && path==paths[current])
        viewport()->repaint();
}

void ImageViewer::unregisterImage(const QString &path)
{
    paths.erase(paths.begin()+paths.indexOf(path));
    imageData.erase(imageData.find(path));
}

void ImageViewer::unregisterImage(int index)
{
    unregisterImage(paths[index]);
}
