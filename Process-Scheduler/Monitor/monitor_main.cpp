#include"project.h"
#include<iostream>
#include<vector>
#include<unistd.h>
#include<cstring>
using namespace Project;
using namespace std;
int main(int argc, char * argv[]){
    for(int i=1;i<argc;i++)
        argv[i-1]=argv[i];
    argv[argc-1]=NULL;
    Monitor job(argv);     
    return 0;
}
