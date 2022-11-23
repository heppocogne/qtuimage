#include "image_xform.h"

ImageXform::ImageXform(QObject *parent, QImage* _image)
    : QObject(parent),
      image(_image)
{

}
