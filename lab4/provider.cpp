#include "provider.h"

#include <stdio.h>
#include <sys/sysinfo.h>


Provider::Provider()
{

}


double Provider::update_cpu_utilization()
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
            return loadavg;
}

 Provider::update_ram()
{
   sysinfo(&info);

}

void Provider::update_process()
{

}
