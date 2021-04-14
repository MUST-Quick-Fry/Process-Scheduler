#include "project.h"
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <iomanip>

using namespace Project;

//RR scheduler with quantum set to 2 seconds
constexpr int QUANT=2;
//
void Scheduler::fileToken(std::string str)
{
    char* token;
    int cnt=0;
    char* strBuf= const_cast<char*>(str.c_str());
  
    token = strtok (strBuf,"\t\n");
    while (token != NULL)
    {
        cnt++;
        if((cnt-1)%3==0){
            std::string buf(token);
            arr_time.push_back(std::stoi(buf));
        }
        else if((cnt-2)%3==0){
            std::string buf(token);
            jobCommand.push_back(buf);
        }
        else if(cnt%3==0){
            std::string buf(token);
            dur_time.push_back(std::stoi(buf));
        }
        else{
            std::invalid_argument("File des is nonstandard!");
        }
        printf ("%s\n",token);
        token = strtok (NULL, "\t\n");
    }
         
}

Scheduler::Scheduler(char * f):fileInfo(Utils::readFile(f))
{
    num=0;
    fileToken(fileInfo);
}

void Scheduler::myPrint(){
    std::cout<<'\n';
    std::cout<<fileInfo;
    std::cout<<'\n';
    for(int i=0;i<arr_time.size();i++){
        std::cout<<arr_time[i]<<' ';
    }
    std::cout<<'\n';
    for(int i=0;i<jobCommand.size();i++){
        std::cout<<jobCommand[i]<<' ';
    }
    std::cout<<'\n';
    for(int i=0;i<dur_time.size();i++){
        std::cout<<dur_time[i]<<' ';
    }
    std::cout<<'\n';
}