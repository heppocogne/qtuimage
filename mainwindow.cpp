#include "mainwindow.h"

#include <QPalette>

using namespace qtuimage;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      viewer(new ImageViewer(this))
{
    setCentralWidget(viewer);
}

MainWindow::~MainWindow()
{
}
