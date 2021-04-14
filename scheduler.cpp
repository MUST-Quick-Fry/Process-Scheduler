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
            jobs[cnt/3].arrive=std::stoi(buf);
            //arr_time.push_back(std::stoi(buf));
        }
        else if((cnt-2)%3==0){
            std::string buf(token);
            jobs[cnt/3].cmd=buf;
            //jobCommand.push_back(buf);
        }
        else if(cnt%3==0){
            std::string buf(token);
             jobs[(cnt/3)-1].duration=std::stoi(buf);
             v_jobs.push_back(jobs[(cnt/3)-1]);
            //dur_time.push_back(std::stoi(buf));
        }
        else{
            throw std::invalid_argument("Job description file is nonstandard!");
        }
        //printf ("%s\n",token);
        token = strtok (NULL, "\t\n");
        
    }
     
}

Scheduler::Scheduler(char * f):fileInfo(Utils::readFile(f))
{
    num=0;
    for(int i=0;i<fileInfo.length();i++){
        if(fileInfo[i]=='\n'){
            num++;
        }
    }
    if(num==0){
        throw std::invalid_argument("Job description file is nonstandard!");
    }
    num++;
    fileToken(fileInfo);
}

void Scheduler::myPrint(){
    std::cout<<'\n';
    std::cout<<fileInfo;
    std::cout<<'\n';
    std::cout<<num<<'\n';
    std::cout<<'\n';
    for(int i=0;i<v_jobs.size();i++){
        std::cout<<v_jobs[i].arrive<<' '<<v_jobs[i].cmd<<' '<<v_jobs[i].duration<<'\n';
    }
}