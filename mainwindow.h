#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include "image_viewer.h"
#include "thumbnails_container.h"

namespace qtuimage
{
    class ImageViewer;

    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        ImageViewer* const viewer;

    private:
        ThumbnailsContainer* const thumbnailsContainer;

        void resizeEvent(QResizeEvent *);

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
    };
};

#endif // MAINWINDOW_H
