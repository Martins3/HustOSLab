#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <iostream>

#include <stdio.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sum  = 0;
    i = 0;
    is_b = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::update_time()
{
    ui->label->setText(QDateTime::currentDateTime().toString("hh:mm:ss"));
}

void MainWindow::update_cpu_utilization()
{
            long double * a;
            long double * b;
            double loadavg;
            FILE *fp;

            if(is_b){
                b = cpu_data[0];
                a = cpu_data[1];
            }else{
                a = cpu_data[0];
                b = cpu_data[1];
            }

            is_b = !is_b;

            fp = fopen("/proc/stat","r");
            fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
            fclose(fp);

            if((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]) == 0){
                loadavg = 0;
            }else{
                loadavg = (double)(((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3])));
            }
            ui->label->setText(QString::number(loadavg));

}


void MainWindow::update_sum()
{
    if(i <= 100) sum = sum + ++i;

    ui->label->setText(QString::number(sum));
}
