#include "thumbnails_container.h"
#include "thumbnail_loader.h"
#include <QDebug>
#include <QSizePolicy>
#include <QWheelEvent>
#include <QScrollBar>

using namespace qtuimage;

QSharedPointer<QIcon> ThumbnailButton::defaultIcon = nullptr;

const QIcon &ThumbnailButton::getDefaultIcon()
{
    if (!defaultIcon)
    {
        auto *pixmap = new QPixmap(ThumbnailLoader::thumbnailSize, ThumbnailLoader::thumbnailSize);
        pixmap->fill(Qt::gray);
        defaultIcon = QSharedPointer<QIcon>(new QIcon(*pixmap));

        delete pixmap;
    }
    return *defaultIcon;
}

ThumbnailButton::ThumbnailButton(QWidget *parent, const QString &_path, QSharedPointer<QPixmap> pixmap)
    : QPushButton(parent),
      path(_path)
{
    if (pixmap)
        setIcon(QIcon(*pixmap));
    else
        setIcon(getDefaultIcon());

    setStyleSheet("border-radius: 0px; border: none;");
    setIconSize(QSize(ThumbnailLoader::thumbnailSize, ThumbnailLoader::thumbnailSize));
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(this, &QPushButton::pressed, this, &ThumbnailButton::onPressed);
}

void ThumbnailButton::onPressed()
{
    emit selected(path.toString());
}

QSize ThumbnailButton::sizeHint(void) const
{
    return QSize(ThumbnailLoader::thumbnailSize, ThumbnailLoader::thumbnailSize);
}

ThumbnailsContainer::ThumbnailsContainer(QWidget *parent, QStringList &_paths)
    : QScrollArea(parent),
      paths(_paths),
      layout(new QHBoxLayout(this))
{
    setVisible(false);

    QWidget *const w = new QWidget(this);
    layout->setContentsMargins(0, 0, 0, 0);
    w->setLayout(layout);
    setWidgetResizable(true);
    setWidget(w);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setBackgroundRole(QPalette::Dark);
}

QSize ThumbnailsContainer::sizeHint(void) const
{
    return QSize(containerCellWidth * paths.size(), containerHeight);
}

void ThumbnailsContainer::wheelEvent(QWheelEvent *event)
{
    auto *const hsb = horizontalScrollBar();
    hsb->setSliderPosition(hsb->sliderPosition() - event->angleDelta().y() / 4);
    emit scrolled(hsb->sliderPosition()/(64+layout->spacing()));
}

void ThumbnailsContainer::addThumbnail(const QString &path, QSharedPointer<QPixmap> pixmap)
{
    auto *b = new ThumbnailButton(this, path, pixmap);
    buttons[path] = b;
    layout->addWidget(b);
    connect(b, &ThumbnailButton::selected, this, &ThumbnailsContainer::onSelected);
}

void ThumbnailsContainer::setIcon(const QString &path, QSharedPointer<QPixmap> pixmap)
{
    buttons[path]->setIcon(QIcon(*pixmap));
}

void ThumbnailsContainer::onSelected(const QString &path)
{
    emit selected(path);
}

void ThumbnailsContainer::onPathsChanged()
{
    for (const auto &newKey : paths)
    {
        if (buttons.count(newKey) == 0)
            addThumbnail(newKey);
    }

    QStringList removed;
    for (auto old = buttons.begin(); old != buttons.end(); old++)
    {
        const auto &oldKey = old.key();
        if (paths.count(oldKey) == 0)
            removed.push_back(oldKey);
    }

    for (const auto &removedKey : removed)
    {
        auto *b = buttons[removedKey];
        layout->removeWidget(b);
        buttons.remove(removedKey);
        delete b;
    }
}
