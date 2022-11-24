#include "mainwindow.h"

#include <QApplication>

#ifdef QT_DEBUG
int main(int argc, char **argv)
{
    QApplication a(argc, argv);
#else

#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int nShowCmd)
{
    QApplication a(__argc, __argv);
#endif

    qtuimage::MainWindow w;
#ifdef QT_DEBUG
    if (1 < argc)
        w.viewer->addPaths(QString::fromLocal8Bit(argv[1]));
#else
    if (1 < __argc)
        w.viewer->addPaths(QString::fromLocal8Bit(__argv[1]));
#endif
    w.show();
    return a.exec();
}
