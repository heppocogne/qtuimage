#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QToolBar>

namespace qtuimage
{
    class ToolBar : public QToolBar
    {
        Q_OBJECT
    public:
        ToolBar(QWidget* parent);
    };
};

#endif // TOOLBAR_H
