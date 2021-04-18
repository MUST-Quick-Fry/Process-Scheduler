#include "project.h"
#include <iostream>
#include <algorithm>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <iomanip>

using namespace Project;

//RR scheduler with quantum set to 2 seconds
constexpr int QUANT=2;

int Scheduler::get_job_num() const
{
    return this->job_num;
}

Scheduler::Scheduler(char * f,char * p):policyInfo(p)
{
    job_num = 0;
    
    auto cho=policyMap.find(policyInfo);
    if(cho==policyMap.end()){
        throw std::invalid_argument("Invalid policy!");
        exit(-1);
    }
    switch (cho->second)
    {
        case Policy::FIFO:
            break;
        case Policy::NONPREEMSJF:
            break;
        case Policy::PREESJF:         
            break;
        case Policy::RR:
            std::cout<<"hh"<<"\n";
            break;
        default:
            throw std::invalid_argument("Invalid policy!");
            break;
    }

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
            job.set_arr_time(std::stoi(buf));
            //job.arrive=std::stoi(buf);
            //arr_time.push_back(std::stoi(buf));
        }
        else if( cnt%3 == 2){
            std::string buf(token);
            job.set_cmd(buf);
            //job.cmd=buf;
            //jobCommand.push_back(buf);
        }
        else{
            Utils::arg_check(token);
            std::string buf(token);
            job.set_dur_time(std::stoi(buf));
            //job.duration=std::stoi(buf);
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

    std::sort(job_queue.begin(),job_queue.end());
    for(auto it =job_queue.begin();it!=job_queue.end();it++)
    {
        std::cout<<it->get_arr_time()<<" "<<it->get_cmd()<<" "<<it->get_dur_time()<<'\n';
    }
    
}
