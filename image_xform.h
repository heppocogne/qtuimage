#ifndef IMAGEXFORM_H
#define IMAGEXFORM_H

#include <QObject>
#include <QImage>
#include <QTransform>

class ImageXform : public QObject
{
    Q_OBJECT
public:
    QImage* image;
    QTransform xform;

    explicit ImageXform(QObject *parent = nullptr, QImage* _image = nullptr);

signals:

};

#endif // IMAGEXFORM_H
