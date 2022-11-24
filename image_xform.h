#ifndef IMAGEXFORM_H
#define IMAGEXFORM_H

#include <QImage>
#include <QTransform>
#include <QSharedPointer>
#include <cmath>

namespace qtuimage
{
    class ImageXform
    {
    public:
        QSharedPointer<QImage> image;
        float scale;
        QPointF local;
        QPointF global;

        enum class ScalingMode
        {
            FIT_WINDOW,
            ACTUAL_SIZE,
            USER_MANIPULATION,
        } scaling_mode;

        explicit ImageXform(QSharedPointer<QImage> _image = nullptr);
        void setLog10Scale(float s) { scale = pow(10, s); }
        float getLog10Scale() const { return log10(scale); }
        QTransform localXform() const;
        QTransform xform() const;
        QRectF getDisplayRect() const;
    };
};

#endif // IMAGEXFORM_H
