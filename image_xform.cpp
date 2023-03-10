#include "image_xform.h"

using namespace qtuimage;

ImageXform::ImageXform(QSharedPointer<QImage> _image)
    : image(_image),
      scale(1.0),
      global(0, 0),
      scalingMode(ScalingMode::FIT_WINDOW)
{
}

QTransform ImageXform::xform() const
{
    return QTransform::fromScale(scale, scale) * QTransform::fromTranslate(global.x(), global.y());
}

void ImageXform::overlapLocalOnGlobal(const QPointF &lp, const QPointF &gp)
{
    global = gp - scale * lp;
}

QRectF ImageXform::getDisplayRect() const
{
    if (image)
        return QRectF(global, scale * image->size());
    else
        return QRectF(global, QSizeF(0, 0));
}
