#include "image_xform.h"

using namespace qtuimage;

ImageXform::ImageXform(QSharedPointer<QImage> _image)
    : image(_image),
      scale(1.0),
      local(0,0),
      global(0,0),
      scaling_mode(ScalingMode::FIT_WINDOW)
{
}

QTransform ImageXform::localXform() const
{
    return QTransform::fromScale(scale, scale) * QTransform::fromTranslate(-local.x(), -local.y());
}

QTransform ImageXform::xform() const
{
    return QTransform::fromTranslate(global.x(), global.y()) * localXform();
}

QRectF ImageXform::getDisplayRect() const
{
    if (image)
        return xform().mapRect(QRectF(QPointF(0, 0), QSizeF(image->size())));
    else
        return QRectF(global, QSizeF(0, 0));
}
