#include "mainwindow.h"

#include <QPalette>
#include <QResizeEvent>

using namespace qtuimage;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      viewer(new ImageViewer(this)),
      thumbnailsContainer(new ThumbnailsContainer(nullptr, viewer->paths)),
      toolbar(new ToolBar(this))
{
    QWidget *const centralWidget = new QWidget(this);
    QVBoxLayout *const l = new QVBoxLayout(centralWidget);
    l->addWidget(toolbar);
    l->addWidget(viewer);
    l->setContentsMargins(QMargins(0, 0, 0, 0));
    centralWidget->setLayout(l);
    setCentralWidget(centralWidget);
    setMouseTracking(true);

    // add "thumbnailsContainer" as a child after "centralWidget"
    thumbnailsContainer->setParent(this);

    connect(viewer, &ImageViewer::pathsChanged, thumbnailsContainer, &ThumbnailsContainer::onPathsChanged);
    connect(viewer, &ImageViewer::mouseMoved, this, &MainWindow::onMouseMoved);
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

void MainWindow::onMouseMoved(QMouseEvent *event)
{
    const auto x = event->pos().x();
    const auto y = event->pos().y();
    const auto w = size().width();
    const auto h = size().height();
    if (0 <= x && x < w && h - ThumbnailsContainer::containerHeight <= y && y < h)
        thumbnailsContainer->setVisible(true);
    else
        thumbnailsContainer->setVisible(false);
}
