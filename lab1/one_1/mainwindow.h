#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    long double cpu_data[2][4];
    bool is_b;
    int sum;
    int i;

public:
    void update_time();
    void update_cpu_utilization();
    void update_sum();
};

#endif // MAINWINDOW_H
