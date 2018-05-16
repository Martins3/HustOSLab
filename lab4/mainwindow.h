#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <sys/sysinfo.h>
#include "provider.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:
    void update_process();
    void update_ram();
    void update_cpu_utilization();

    void update_run_time();
private:
    Ui::MainWindow *ui;

    int i;
    struct sysinfo info;
    long double cpu_data[2][4];
    bool is_b;

};

#endif // MAINWINDOW_H
