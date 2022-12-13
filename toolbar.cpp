#include "toolbar.h"
#include <QAction>
#include <QFileDialog>
#include <QDir>
#include <QStandardPaths>

using namespace qtuimage;

ToolBar::ToolBar(QWidget *parent)
    : QToolBar(parent),
      menu(new QMenu(this))
{
    auto* const openFile = menu->addAction("ファイルを開く", this, &ToolBar::openFileSelector);
    auto* const openFolder = menu->addAction("フォルダーを開く", this, &ToolBar::openFolderSelector);
    auto* const quitApp = menu->addAction("終了", this, &ToolBar::quitApp);
    auto* const fileAction = addAction("ファイル", this, &ToolBar::popupMenu);
}

void ToolBar::popupMenu()
{
    menu->popup(mapToGlobal(geometry().bottomLeft()));
}

void ToolBar::openFileSelector()
{
    const QString selected=QFileDialog::getOpenFileName(this,
                               "Open File",
                               "",
                               "Images (*.png *.jpg *.bmp *.gif *.svg)");

    if(selected!="")
        emit pathSelected(selected);
}

void ToolBar::openFolderSelector()
{
    const QString selected=QFileDialog::getExistingDirectory(this,
                               "Open Folder",
                               "");

    if(selected!="")
        emit pathSelected(selected);
}

void ToolBar::quitApp()
{
    emit quitRequest();
}
