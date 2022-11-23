#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QImage>

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QStringList paths;
    QList<QImage> thumbnails;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif // MAINWINDOW_H
