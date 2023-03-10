#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include "image_viewer.h"
#include "thumbnails_container.h"
#include "toolbar.h"

namespace qtuimage
{
    class ImageViewer;

    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        ImageViewer *const viewer;

    private:
        ThumbnailsContainer *const thumbnailsContainer;
        ToolBar *const toolbar;

        void resizeEvent(QResizeEvent *event);
        void closeEvent(QCloseEvent *);
        void onQuitApp();

    private slots:
        void onMouseMoved(QMouseEvent *event);
        void quitRequested();

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
    };
};

#endif // MAINWINDOW_H
