
#include "mainwindow.h"
#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    QTimer * timer = new QTimer();
    MainWindow::connect(timer, &QTimer::timeout, &w, &MainWindow::update_ram);
    MainWindow::connect(timer, &QTimer::timeout, &w, &MainWindow::update_cpu_utilization);
    MainWindow::connect(timer, &QTimer::timeout, &w, &MainWindow::update_process);
    MainWindow::connect(timer, &QTimer::timeout, &w, &MainWindow::update_run_time);
    timer->start(1000);


    return a.exec();
}
