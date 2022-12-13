#include "mainwindow.h"

#include <QPalette>
#include <QResizeEvent>

using namespace qtuimage;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      viewer(new ImageViewer(this)),
      thumbnailsContainer(new ThumbnailsContainer(this, viewer->paths))
{
    setCentralWidget(viewer);
    setMouseTracking(true);
    connect(viewer, &ImageViewer::pathsChanged, thumbnailsContainer, &ThumbnailsContainer::onPathsChanged);
    connect(viewer, &ImageViewer::thumbnailRegistered, thumbnailsContainer, &ThumbnailsContainer::setIcon);
    connect(thumbnailsContainer, &ThumbnailsContainer::selected, viewer, &ImageViewer::onThumbnailSelected);
}

MainWindow::~MainWindow()
{
    delete viewer;
    delete thumbnailsContainer;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    thumbnailsContainer->setGeometry(QRect(QPoint(0, event->size().height() - ThumbnailsContainer::containerHeight), QSize(event->size().width(), ThumbnailsContainer::containerHeight)));
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    const auto x=event->pos().x();
    const auto y=event->pos().y();
    const auto w=size().width();
    const auto h=size().height();
    if(0<=x && x<w && h-ThumbnailsContainer::containerHeight<=y && y<h)
        thumbnailsContainer->setVisible(true);
    else
        thumbnailsContainer->setVisible(false);
}
