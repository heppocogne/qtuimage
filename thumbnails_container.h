#ifndef THUMBNAILSCONTAINER_H
#define THUMBNAILSCONTAINER_H

#include <QScrollArea>
#include <QSharedDataPointer>
#include <QString>
#include <QPixmap>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMap>
#include "thumbnail_loader.h"

namespace qtuimage
{
    class ThumbnailButton : public QPushButton
    {
        Q_OBJECT

        const QStringView path;
        static QSharedPointer<QIcon> defaultIcon;
        static const QIcon &getDefaultIcon();

    private slots:
        void onPressed();

    public:
        ThumbnailButton(QWidget *parent, const QString &_path, QSharedPointer<QPixmap> pixmap = nullptr);
        QSize sizeHint(void) const;

    signals:
        void selected(const QString &path);
    };

    class ThumbnailsContainer : public QScrollArea
    {
        Q_OBJECT

        QStringList &paths;
        QHBoxLayout *const layout;

        QMap<QString, ThumbnailButton *> buttons;

    private slots:
        void onSelected(const QString &path);

    public:
        static const constexpr auto containerCellWidth = ThumbnailLoader::thumbnailSize;
        static const constexpr auto containerHeight = 96;

        ThumbnailsContainer(QWidget *parent, QStringList &_paths);

        void addThumbnail(const QString &path, QSharedPointer<QPixmap> pixmap = nullptr);
        QSize sizeHint(void) const;
        void wheelEvent(QWheelEvent *event);

    public slots:
        void setIcon(const QString &path, QSharedPointer<QPixmap> pixmap);
        void onPathsChanged();
        void onSliderValueChanged(int value);

    signals:
        void selected(const QString &path);
        void scrolled(int left);
    };
};

#endif // THUMBNAILSCONTAINER_H
