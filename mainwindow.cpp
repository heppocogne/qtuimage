#include "mainwindow.h"

#include <QPalette>
#include <QResizeEvent>
#include <QCoreApplication>

#include "configure.h"

using namespace qtuimage;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      viewer(new ImageViewer(this)),
      thumbnailsContainer(new ThumbnailsContainer(nullptr, viewer->paths)),
      toolbar(new ToolBar(this))
{
    // restore geometry
    auto *const cfg = Configure::getSingleton();
    setGeometry(QRect(
                    cfg->get("window.pos_x",60),
                    cfg->get("window.pos_y",45),
                    cfg->get("window.width", 1200),
                    cfg->get("window.height", 900)
                ));
    QWidget *const centralWidget = new QWidget(this);
    QVBoxLayout *const l = new QVBoxLayout(centralWidget);
    l->addWidget(toolbar);
    l->addWidget(viewer);
    l->setContentsMargins(QMargins(0, 0, 0, 0));
    l->setSpacing(0);
    centralWidget->setLayout(l);
    setCentralWidget(centralWidget);
    setMouseTracking(true);

    // add "thumbnailsContainer" as a child after "centralWidget"
    thumbnailsContainer->setParent(this);

    connect(viewer, &ImageViewer::pathsChanged, thumbnailsContainer, &ThumbnailsContainer::onPathsChanged);
    connect(viewer, &ImageViewer::mouseMoved, this, &MainWindow::onMouseMoved);
    connect(viewer, &ImageViewer::thumbnailRegistered, thumbnailsContainer, &ThumbnailsContainer::setIcon);
    connect(toolbar, &ToolBar::pathSelected, viewer, &ImageViewer::addPaths);
    connect(toolbar, &ToolBar::quitRequest, this, &MainWindow::quitRequested);
    connect(thumbnailsContainer, &ThumbnailsContainer::selected, viewer, &ImageViewer::onThumbnailSelected);
    connect(thumbnailsContainer, &ThumbnailsContainer::scrolled, viewer, &ImageViewer::onThumbnailScrolled);
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

void MainWindow::closeEvent(QCloseEvent *)
{
    onQuitApp();
}

void MainWindow::quitRequested()
{
    onQuitApp();
    QCoreApplication::exit(0);
}

void MainWindow::onQuitApp()
{
    const auto& geo=geometry();
    auto *const cfg = Configure::getSingleton();
    // remember geometry
    cfg->set("window.width", geo.width());
    cfg->set("window.height", geo.height());
    cfg->set("window.pos_x", geo.x());
    cfg->set("window.pos_y", geo.y());
    Configure::removeSingleton();
}
