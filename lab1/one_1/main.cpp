#include "mainwindow.h"
#include <QApplication>
#include <QTimer>

#include <iostream>

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char *argv[])
{
    pid_t child_one;
    pid_t child_two;
    pid_t child_three;

                QApplication a(argc, argv);
    if((child_one = fork()) == 0) {
        if((child_two = fork()) == 0){
            if((child_three = fork()) == 0){
                return 0;
            }else{
                std::cout << "child_three start" << std::endl;
                MainWindow w;
                w.show();
                QTimer * timer = new QTimer();
                MainWindow::connect(timer, &QTimer::timeout, &w, &MainWindow::update_sum);
                timer->start(3000);
                return a.exec();
            }
        }else{
                std::cout << "child_two start" << std::endl;
//                QApplication a(argc, argv);
                MainWindow w;
                w.show();
                QTimer * timer = new QTimer();
                MainWindow::connect(timer, &QTimer::timeout, &w, &MainWindow::update_cpu_utilization);
                timer->start(2000);
                return a.exec();
        }
    }else{
                std::cout << "child_one start" << std::endl;
//                QApplication a(argc, argv);
                MainWindow w;
                w.show();
                QTimer * timer = new QTimer();
                MainWindow::connect(timer, &QTimer::timeout, &w, &MainWindow::update_time);
                timer->start(1000);
                return a.exec();
    }




    return 0;
}

