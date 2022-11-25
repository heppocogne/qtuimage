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
    connect(viewer, &ImageViewer::pathsChanged, thumbnailsContainer, &ThumbnailsContainer::onPathsChanged);
    connect(viewer, &ImageViewer::thumbnailRegistered, thumbnailsContainer, &ThumbnailsContainer::setIcon);
    connect(thumbnailsContainer, &ThumbnailsContainer::selected, viewer, &ImageViewer::onThumbnailSelected);
}

MainWindow::~MainWindow()
{
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    thumbnailsContainer->setGeometry(QRect(QPoint(0,event->size().height()-ThumbnailsContainer::containerHeight),QSize(event->size().width(),ThumbnailsContainer::containerHeight)));
}
