#include <stdio.h>
#include "../header/config.h"
#include "../header/kernel.h"
#include "../header/io.h"
#include "../header/app.h"
#include "../header/debug.h"
#include "../header/tools.h"

int main(){
    install_fs();
    fs_boot();
    if(0){
        RE_INSTALL = 1;
        fs_shutdown();
        install_fs();
        fs_boot();

        touch("a");
        touch("b");
        mkdir("c");

        cd("c");
        touch("a");
        touch("b");
        mkdir("c");
        cd("c");
        cd("/");

        tree();
    
        rm("c", IS_DIR);
        ls(0);
    }else{
        
        terminal();
    }
    
    fs_shutdown();
    return 0;
}
