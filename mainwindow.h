#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "image_viewer.h"

namespace qtuimage
{
    class ImageViewer;

    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        ImageViewer *viewer;

        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
    };
};

#endif // MAINWINDOW_H
