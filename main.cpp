#include"project.h"
#include<iostream>
#include<vector>
#include<unistd.h>
#include<cstring>
#include<algorithm>
using namespace Project;
using namespace std;
int main(int argc, char * argv[]){
    if(argc!=3){
        throw invalid_argument("The number of arguments of Scheduler is nonstandard!");
    }
    Scheduler sc(argv[1],argv[2]);   
    //cout<<sc; 
    //sc.Display();
    return 0;
}
