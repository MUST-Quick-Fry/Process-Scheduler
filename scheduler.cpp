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

Scheduler::Scheduler(char * f)
{
    job_num = 0;
    std::string tmp = Utils::readFile(f);
    normalizeCheck(tmp);
    splitToken(tmp);
}

void Scheduler::normalizeCheck(std::string cont){
        
    int arg_num = 0; 
    
    for(int i = 0; i < cont.length(); ++i){
        if(cont[i]=='\n'){
        
            if(cont[i-1]=='\n'){ 
                if(cont[i-2]=='\t'){
                    throw std::invalid_argument("Argument Error: Empty argument!");
                }
    	        continue;
            }
            
            if(cont[i-1]=='\t'){
                throw std::invalid_argument("Argument Error: Empty argument!");
            }
              
            arg_num++;
            if(arg_num > 3){
                throw std::invalid_argument("Argument Error: Job description file has redundant arguments!");
            }
            if(arg_num < 3){
                throw std::invalid_argument("Argument Error: Job description file has less arguments!");
            }
            arg_num = 0;
            job_num++;
        }
        if(cont[i]=='\t'){arg_num++;}
    }
    
}

void Scheduler::splitToken(std::string str)
{
    int cnt=0;
    char* strBuf= const_cast<char*>(str.c_str());
  
    char* token = strtok (strBuf,"\t\n");
    while (token != NULL)
    {
	cnt++;
        if(cnt %3 == 1){
            Utils::arg_check(token);
            std::string buf(token);
            job.arrive=std::stoi(buf);
            //arr_time.push_back(std::stoi(buf));
        }
        else if( cnt%3 == 2){
            std::string buf(token);
            job.cmd=buf;
            //jobCommand.push_back(buf);
        }
        else{
            Utils::arg_check(token);
            std::string buf(token);
            job.duration=std::stoi(buf);
            job_queue.push_back(job);
            //dur_time.push_back(std::stoi(buf));
        }
        
        //printf ("%s\n",token);
        token = strtok (NULL, "\t\n");
        
    }
     
}

void Scheduler::Display(){
    std::cout<<"Totally " << job_num << " jobs\n";
    std::cout<<'\n';
    std::cout<<"Gantt Chart"<<'\n';
    std::cout<<"======================================"<<'\n';
    
    for(unsigned i=0;i<job_queue.size();i++){
        std::cout<<job_queue[i].arrive<<" "<<job_queue[i].cmd<<" "<<job_queue[i].duration<<'\n';
    }
}
