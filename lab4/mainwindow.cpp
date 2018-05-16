#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <math.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <QStringList>
#include <QStringListModel>
#include <dirent.h>
#include <QDebug>
#include <vector>

#include <stdio.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <errno.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    is_b = false;
    i = 0;


    ui->cpu->setMinimum(0);
    ui->cpu->setMaximum(100);
    ui->ram->setMinimum(0);
    ui->ram->setMaximum(100);

    // show the static message

    FILE * f = fopen("/proc/sys/kernel/hostname", "r");
    char host_name[100];
    fscanf(f, "%s", host_name);
    QString s_1 = "host_name : ";
    s_1 += QString::fromUtf8(host_name);
    ui->label->setText(s_1);

       struct sysinfo info;
        time_t cur_time = 0;
        time_t boot_time = 0;
        struct tm *ptm = NULL;
        if (sysinfo(&info)) {
            fprintf(stderr, "Failed to get sysinfo, errno:%u, reason:%s\n",
            errno, strerror(errno));
            return;
        }
        time(&cur_time);
        if (cur_time > info.uptime) {
            boot_time = cur_time - info.uptime;
        }else {
            boot_time = info.uptime - cur_time;
        }
        ptm = localtime(&boot_time);
        char start_time[200];
        sprintf(start_time, "System boot time: %d-%-d-%d %d:%d:%d\n", ptm->tm_year + 1900,
            ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

    QString s_3 = QString::fromUtf8(start_time);
    ui->label_5->setText(s_3);




    struct utsname name_buf;
    uname(&name_buf);
    QString s_4;
    s_4 += QString::fromUtf8(name_buf.sysname) + "\n" + QString::fromUtf8(name_buf.machine);
    s_4 += "\n" + QString::fromUtf8(name_buf.version);
    ui->label_6->setText(s_4);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_run_time(){
    QString s_2 = "Run time : ";
    FILE * f = fopen("/proc/uptime", "r");
    double d_up_time;
    fscanf(f, "%lf", &d_up_time);
    int up_time = (int)d_up_time;
    int mins =  (up_time / 60) % 60;
    int hours = up_time / (60 * 60);
    int secs = up_time % 60;
    s_2 = s_2 + QString::number(hours) + "h : " + QString::number(mins)+ "m : " + QString::number(secs) + "s";
    ui->label_2->setText(s_2);
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

            ui->cpu->setValue((int)(round(loadavg * 100)));
            ui->cpu_info->setText("CPU:" + QString::number(loadavg, 'f', 2));
}

void MainWindow::update_ram()
{
   sysinfo(&info);
   unsigned long  total = info.totalram;
   unsigned long free = info.freeram;
   double t = 1.0 / 1024 / 1024 / 1024;
   ui->ram_info->setText(QString::number(free * t, 'f', 2) + "GB/" + QString::number(total * t, 'f', 2) + "GB");
   ui->ram->setValue(free * 100 / total);
}



long process_size_in_pages(int pid)
{

   char filename[1000];
   sprintf(filename, "/proc/%d/statm", pid);
   FILE *f = fopen(filename, "r");

   long s = -1;
   if (!f) return -1;
   fscanf(f, "%ld", &s);
   fclose (f);
   return s;
}

void MainWindow::update_process()
{
    // get all the pid

    QStringList* stringList = new QStringList();
    stringList->append("pid\t comm\t\t state\t\t ppid\t\t pages");


    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir ("/proc/")) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            bool is_pid = true;
            int i = 0;
            while(ent->d_name[i] != '\0'){
                if(!isdigit(ent->d_name[i])){
                    is_pid = false;
                    break;
                }
                i++;
            }
            if(is_pid){
                int pid = atoi(ent->d_name);


        char filename[1000];
        sprintf(filename, "/proc/%d/stat", pid);
        FILE *f = fopen(filename, "r");

        int unused;
        char comm[1000];
        char state;
        int ppid;
        fscanf(f, "%d %s %c %d", &unused, comm, &state, &ppid);
        fclose(f);

        char content[1000];
        sprintf(content, "%d\t", pid);
        sprintf(content + strlen(content), "%s\t\t", comm);
        sprintf(content + strlen(content), "%c\t\t", state);
        sprintf(content + strlen(content), "%d\t\t", ppid);
        sprintf(content + strlen(content), "%ld\t\t", process_size_in_pages(pid));


//        QString pid_info = QString::number(pid) + "\t";
//        pid_info += QString::fromUtf8(comm) + "\t\t";
//        pid_info += QChar(state);
//        pid_info += "\t\t";

//        pid_info += QString::number(ppid) + "\t\t";
//        pid_info += QString::number(process_size_in_pages(pid));
        QString pid_info = QString::fromUtf8(content);
        stringList->append(pid_info);

            }
        }

        closedir (dir);
    }else{
        printf("failed\n");
    }

    QStringListModel* listModel = new QStringListModel(*stringList, NULL);
    ui->listView->setModel(listModel);
}
