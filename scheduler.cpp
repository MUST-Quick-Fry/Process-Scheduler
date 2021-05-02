#include "project.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <algorithm>
#include <iomanip>
#include <queue>

using namespace Project;

void Scheduler::set_total_time(int now)
{
    this->total_time = now;
}

int Scheduler::get_total_time() const
{
    return this->total_time;
}

int Scheduler::get_job_num() const
{
    return this->job_num;
}

Scheduler::Scheduler(char *f, char *p) : policyInfo(p)
{
    std::string tmp = Utils::readFile(f);
    normalizeCheck(tmp);
    splitToken(tmp);
    job_num = job_queue.size();
    choosePolicy();
    //std::sort(job_queue.begin(),job_queue.end());
    //schedulerDrive();
}

void Scheduler::normalizeCheck(std::string cont)
{

    int arg_num = 0;

    for (int i = 0; i < cont.length(); ++i)
    {
        if (cont[i] == '\n')
        {

            if (cont[i - 1] == '\n')
            {
                if (cont[i - 2] == '\t')
                {
                    throw std::invalid_argument("Argument Error: Empty argument!");
                }
                continue;
            }

            if (cont[i - 1] == '\t')
            {
                throw std::invalid_argument("Argument Error: Empty argument!");
            }

            arg_num++;
            if (arg_num > 3)
            {
                throw std::invalid_argument("Argument Error: Job description file has redundant arguments!");
            }
            if (arg_num < 3)
            {
                throw std::invalid_argument("Argument Error: Job description file has less arguments!");
            }
            arg_num = 0;
        }
        if (cont[i] == '\t')
        {
            arg_num++;
        }
    }
}

void Scheduler::splitToken(std::string str)
{
    int cnt = 0;
    char *strBuf = const_cast<char *>(str.c_str());

    char *token = strtok(strBuf, "\t\n");
    Job job = Job();
    while (token != NULL)
    {
        cnt++;
        if (cnt % 3 == 1)
        {
            job.ID = cnt/3;
            Utils::arg_check(token);
            std::string buf(token);
            job.set_arr_time(std::stoi(buf));
        }
        else if (cnt % 3 == 2)
        {
            std::string buf(token);
            job.set_cmd(buf);
        }
        else
        {
            Utils::arg_check(token);
            std::string buf(token);
            job.set_dur_time(std::stoi(buf));
            job_queue.push_back(job);
        }
        token = strtok(NULL, "\t\n");
    }
}

void Scheduler::choosePolicy()
{
    sort(job_queue.begin(), job_queue.end());
    std::cout<< "Totally "<<job_queue.size()<<" jobs are ready!" <<'\n';
    auto cho = policyMap.find(policyInfo);
    if (cho == policyMap.end())
    {
        throw std::invalid_argument("Invalid policy!");
        exit(-1);
    }
    switch (cho->second)
    {
    case Policy::FIFO:
        /* FIFO function interface*/
        driveFIFO();
        break;
    case Policy::NONPREEMSJF:
        /*non-preetive SJF function interface*/
        driveSJF1();
        break;
    case Policy::PREESJF:
        /*preetive SJF function interface*/
        driveSJF2();
        break;
    case Policy::RR:
        /*RR function interface*/
        driveRR();
        break;
    default:
        throw std::invalid_argument("Invalid policy!");
        break;
    }
    //std::cout << *this;
}

void Scheduler::driveFIFO()
{
    std::priority_queue<Job,std::vector<Job>,std::less<Job> > pq_arr;
    int len = get_job_num();
    int time = 0;
     
    for(int i = 0; i < len; ++i){
        pq_arr.emplace(job_queue[i]);  
    }
    
    time = pq_arr.top().get_arr_time();
    
    while(!pq_arr.empty()){
        auto it = pq_arr.top();
        time += it.get_dur_time();
        scheduler.emplace(it);
        pq_arr.pop();
    }
    
    // display
    set_total_time(time);
    Display(scheduler);
    std::cout << std::endl;
    
    // execute
    int realtime = 0;
    int pid = 1;
   
    while(!stop_flag){
    
        //std::cout << "now time: " << realtime << std::endl;    
        
        while(!scheduler.empty() && realtime == scheduler.front().get_arr_time()){
            auto it = scheduler.front();
            scheduler.pop();
            
            if(monitor_map[it.ID]==0){
                pid = fork();
                if(pid == 0){
                    Monitor* monitor = new Monitor(it);                  
                }
                else{
                    monitor_map[it.ID]=pid;
                }
            }
            
            
            if(pid !=0){     
                                           
                     if(allow_preem){                 
                         allow_preem = false;                                  
                         this_job = it;
                         
                         
                         signal(SIGALRM, job_FIFO);
                         alarm(it.get_dur_time());
                               
                         sleep(0.1);                
                         kill(monitor_map[it.ID], SIGCONT);    

                     }
                     else{
                         wait_queue.emplace(it);
                     }
                 
             }
        }
        
        
        if(pid!=0){         
             sleep(1);          
             realtime++;
             
        }
                        
    }
    
    if(pid !=0){ 
        while (1) {
            int result = wait(NULL);
            if (result == -1) {
                if (errno == EINTR) { continue;}

                break;
            }
        }

    }
    
}

void Scheduler::driveSJF1()
{
    
}

void Scheduler::driveRR(){
    
    std::priority_queue<Job,std::vector<Job>,std::less<Job> > pq_arr;
    std::queue<Job> wait_q; 
    int len = get_job_num();
     
    for(int i = 0; i < len; ++i){
        pq_arr.emplace(job_queue[i]);  
    }
    
    // create a schedule
    int time = 0;
    bool stop = false;
   
    while(!stop){
        if(pq_arr.empty() || time < pq_arr.top().get_arr_time()){
            if(wait_q.empty()){time++;}
            else{
                Job tmp2 = wait_q.front();
                wait_q.pop();
                tmp2.set_arr_time(time);
                if(tmp2.get_dur_time() > QUANT){
                    tmp2.service_time_left = tmp2.get_dur_time();
                    scheduler.emplace(tmp2);
                    tmp2.set_dur_time(tmp2.get_dur_time()-QUANT);
                    wait_q.emplace(tmp2);
                    time += QUANT;
                }
                else{
                    tmp2.service_time_left = tmp2.get_dur_time();
                    scheduler.emplace(tmp2);
                    time += tmp2.get_dur_time();
                }
            }
        }
        else{
            Job tmp = pq_arr.top();
            pq_arr.pop(); 
            
            if(tmp.get_dur_time() > QUANT){
                 tmp.service_time_left = tmp.get_dur_time();
                 scheduler.emplace(tmp);
                 scheduler.back().set_dur_time(QUANT);
                 tmp.set_dur_time(tmp.get_dur_time()-QUANT);
                 wait_q.emplace(tmp);
                 time += QUANT;
             }
             else{
                 tmp.service_time_left = tmp.get_dur_time();
                 scheduler.emplace(tmp);
                 time += tmp.get_dur_time();
             }
        
        }
        
        if(wait_q.empty() && pq_arr.empty()){stop = true;}
    }
    
    // draw scheduler
    set_total_time(time);
    Display(scheduler);
    std::cout << std::endl;
    
    /*
    
    // display
    while(!scheduler.empty()){
        std::cout << scheduler.front().ID << " " << scheduler.front().get_arr_time() << " " << scheduler.front().service_time_left << std::endl;
        scheduler.pop();
    }
    
    */
    
    // execute
    int realtime = 0;
    int pid = 1;
        
    while(!stop_flag)
    {
    
         std::cout << "now time: " << realtime << std::endl;    
         
         while(!scheduler.empty() && realtime == scheduler.front().get_arr_time()){
                auto it = scheduler.front();                      
                scheduler.pop();
             
                if(monitor_map[it.ID]==0){
                     pid = fork();
                     if(pid == 0){
                         Monitor* monitor = new Monitor(it);                  
                     }
                     else{
                         monitor_map[it.ID]=pid;
                     }
                 }
                  
                 if(pid !=0){                                   
                     if(allow_preem){                 
                         allow_preem = false;                                  
                         this_job = it;
                         
                         signal(SIGALRM, job_stop);
                         alarm(it.get_dur_time());
                            
                         sleep(0.1);                
                         kill(monitor_map[it.ID], SIGCONT);    
                            
                         //std::cout << "allo_preem " << allow_preem<<std::endl;
                     }
                     else{
                         wait_queue.emplace(it);
                     }
                 
                 }
 
         }
         
         if(pid!=0){      
             sleep(1);            
             realtime++;

         }
                        
    }
    
    if(pid !=0){ 
        while (1) {
            int result = wait(NULL);
            if (result == -1) {
                if (errno == EINTR) { continue;}

                break;
            }
        }

    }
        
}

void Scheduler::driveSJF2(){
    
}

namespace Project
{
    std::ostream &operator<<(std::ostream &out, const Scheduler &sc)
    {
        out << "End" <<'\n';
        //out << "Totally " << sc.get_job_num() << " jobs\n";
        //out << '\n';
        //out << "-----------------------------------------------------------------------------------------------" << '\n';
        //out << "Gantt Chart" << '\n';
        //out << "-----------------------------------------------------------------------------------------------" << '\n';
        //out << "Time" << std::setfill(' ') << std::setw(8) << '|';
        //for (int i = 0; i <= sc.get_total_time() / 10; i++)
        //{
            //out << i;
            //out << std::setfill(' ') << std::setw(20);
        //}
        //out << '\n';
        //out << std::setfill(' ') << std::setw(12) << '|';
        //for (int i = 0; i <= sc.get_total_time() / 10; i++)
        //{
            //for (int j = 0; j <= 9; j++)
            //{
                //out << j << ' ';
            //}
        //}
        //out << '\n';
        //for (int i = 0; i < sc.get_job_num(); i++)
        //{
            //out << "Job " << i + 1 << std::setfill(' ') << std::setw(7) << '|';
            //for(int j=0;j<sc.job_queue[i].get_arr_time()-sc.job_queue[i].get_wait_time()
            //;j++){
                //out<<"  ";
            //}
            //for(int j=0;j<sc.job_queue[i].get_wait_time();j++)
            //{
                //out<<". ";
            //}
            //for (int j = 0; j < sc.job_queue[i].get_dur_time(); j++)
            //{
                //out << "# ";
            //}
            //out << '\n';
        //}
        //out << "Mixed" << std::setfill(' ') << std::setw(7) << '|';
        //for(int i=0;i<sc.job_queue[0].get_arr_time();i++)
        //{
            //out<<"  ";
        //}
        //out << std::setfill(' ') << std::setw(sc.job_queue[0].get_arr_time() * 2);
        //for (int i = 0; i < sc.get_job_num(); i++)
        //{
            //for (int j = 0; j < sc.job_queue[i].get_dur_time(); j++)
            //{
                //out << i + 1 << ' ';
            //}
        //}
        //out << '\n';
    }
}

void Scheduler::Display(std::queue<Job> q)
{
    std::cout<<"Totally " << get_job_num() << " jobs, Time using: " << get_total_time()<<" s\n";
    std::cout<<"======================================================================="<<'\n';
    std::cout<<"Gantt Chart"<<'\n';
    std::cout<<"======================================================================="<<'\n';
    std::cout << "Time" << std::setfill(' ') << std::setw(5) << "|";
     
    for (int i = 0; i <= get_total_time()/ 10; i++)
    {
        std::cout << i;
        std::cout << std::setfill(' ') << std::setw(20);
    }   
        
    std::cout << '\n';
    std::cout << std::setfill(' ') << std::setw(9) << '|';
    for (int i = 0; i <= get_total_time() / 10; i++)
    {
        for (int j = 0; j <= 9; j++){ std::cout << j << ' ';}
    }
    std::cout << '\n';
    
    int stopindex = 0;
    
    while(!q.empty())
    {
         auto it = q.front();
         q.pop();
         
         std::cout<<"Job " << it.ID <<std::setw(4) << "|";
         int arr_time = it.get_arr_time();
         int dur_time = it.get_dur_time();
         for(int i=0; i<arr_time; ++i){ std::cout << " " << ' ';}
         std::cout << "." << ' '; // arrive
         for(int i=arr_time + 1; i<=get_total_time(); ++i){ 
             if(i <= stopindex){std::cout << "." << ' ';}
             else{
                 std::cout << "#" << ' ';
                 dur_time--;
                 if(dur_time==0){stopindex = i; break;}
             }
         }
         for(int i=stopindex + 1; i<=get_total_time(); ++i){ std::cout << " " << ' ';}
         std::cout << std::endl;
    }
}


