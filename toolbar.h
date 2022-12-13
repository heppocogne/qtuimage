#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QToolBar>
#include <QMenu>

namespace qtuimage
{
    class ToolBar : public QToolBar
    {
        Q_OBJECT

        QMenu* const menu;
    public:
        ToolBar(QWidget *parent);

    private slots:
        void popupMenu();
        void openFileSelector();
        void openFolderSelector();
        void quitApp();

    signals:
        void pathSelected(const QString& path);
        void quitRequest();
    };
};

#endif // TOOLBAR_H
