#ifndef PROVIDER_H
#define PROVIDER_H

#include <sys/sysinfo.h>

class Provider
{
public:
    Provider();


private:
    // cpu data
    long double cpu_data[2][4];
    bool is_b;
    void update_cpu_utilization();

    // ram
    struct sysinfo info;
    void update_ram();


    // process
    void update_process();
};

#endif // PROVIDER_H
