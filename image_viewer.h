#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QGraphicsView>
#include <QStringList>
#include <QPixmap>
#include <QSharedPointer>
#include "image_xform.h"

namespace qtuimage
{
    class ImageViewer : public QGraphicsView
    {
        Q_OBJECT

        const QStringList supportedExtensions = {
            ".bmp",
            ".gif",
            ".jpg",
            ".jpeg",
            ".png",
            ".pbm",
            ".pgm",
            ".ppm",
            ".xbm",
            ".xpm",
            ".svg",
        };
        struct ImageData
        {
            QSharedPointer<QPixmap> thumbnail = nullptr;
            QSharedPointer<ImageXform> main = nullptr;
        };
        QMap<QString, QSharedPointer<ImageData>> imageData;

        bool leftClick;
        QPoint mousePosition;
        int current;

        bool isCurrentReady() const;

        QSize sizeHint(void) const;

        void keyPressEvent(QKeyEvent *event);
        void keyReleaseEvent(QKeyEvent *event);
        void paintEvent(QPaintEvent *);
        void mouseMoveEvent(QMouseEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void resizeEvent(QResizeEvent *);
        void wheelEvent(QWheelEvent *event);

        void invokeRepaint();
        void adjustImageScale();
        void adjustImagePosition();

    public:
        QStringList paths;

        ImageViewer(QWidget *parent = nullptr);

        void addPaths(const QString &path);
        void addPathsRecursive(const QString &path);
        void addImage(const QString &path);
        void removeImage(const QString &path);
        void removeImage(int index);

    public slots:
        void registerImage(const QString &path, QSharedPointer<QImage> image);
        void registerThumbnail(const QString &path, QSharedPointer<QPixmap> pixmap);
        void onThumbnailSelected(const QString &path);

    signals:
        void pathsChanged();
        void thumbnailRegistered(const QString &path, QSharedPointer<QPixmap> pixmap);
        void mouseMoved(QMouseEvent *event);
    };
};

#endif // IMAGEVIEWER_H
