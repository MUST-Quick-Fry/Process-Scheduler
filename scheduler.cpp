#include "project.h"
#include <iostream>
#include <algorithm>
#include <vector>
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
    std::string tmp = Utils::readFile(f);
    normalizeCheck(tmp);
    splitToken(tmp);
    choosePolicy();
    std::sort(job_queue.begin(),job_queue.end());
    //schedulerDrive();
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
        }
        else if( cnt%3 == 2){
            std::string buf(token);
            job.set_cmd(buf);
        }
        else{
            Utils::arg_check(token);
            std::string buf(token);
            job.set_dur_time(std::stoi(buf));
            job_queue.push_back(job);
        }
        token = strtok (NULL, "\t\n");
        
    }
     
}


void Scheduler::schedulerDrive()
{
    
    // for(auto it=job_queue.begin();it!=job_queue.end();it++)
    // {
    //     int process;
    //     int status;

    //     process = fork();
    //     it->set_pid(process);
    //     it->set_state(status);
    //     if(process < 0){
    //     // error
    //         status = -1;
    //     }
    //     else if(process == 0){ 
    //         // child process
    //         monitor(it->get_job());
    //         throw std::invalid_argument("Fail to execute Child Process!\n");
    //     }
    //     else{
    //     // parent process
            

    //     // quit loop after child process terminated!
    //     }

    // }
}

void Scheduler::choosePolicy()
{
    auto cho=policyMap.find(policyInfo);
    if(cho==policyMap.end()){
        throw std::invalid_argument("Invalid policy!");
        exit(-1);
    }
    switch (cho->second)
    {
        case Policy::FIFO:
            /* FIFO function interface*/
            break;
        case Policy::NONPREEMSJF:
            /*non-preetive SJF function interface*/
            break;
        case Policy::PREESJF:
             /*preetive SJF function interface*/         
            break;
        case Policy::RR:
            /*RR function interface*/
            std::cout<<"hh"<<"\n";
            break;
        default:
            throw std::invalid_argument("Invalid policy!");
            break;
    }

}

void Scheduler::Display(){
    std::cout<<"Totally " << job_num << " jobs\n";
    std::cout<<'\n';
    std::cout<<"Gantt Chart"<<'\n';
    std::cout<<"======================================"<<'\n';

    
    for(auto it =job_queue.begin();it!=job_queue.end();it++)
    {
        std::cout<<it->get_arr_time()<<" "<<it->get_cmd()<<" "<<it->get_dur_time()<<'\n';
    }
    
}
