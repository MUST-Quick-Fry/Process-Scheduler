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
using namespace std;

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
    string tmp = Utils::readFile(f);
    normalizeCheck(tmp);
    splitToken(tmp);
    job_num = job_queue.size();
    choosePolicy();
    //sort(job_queue.begin(),job_queue.end());
    //schedulerDrive();
}

void Scheduler::normalizeCheck(string cont)
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
                    throw invalid_argument("Argument Error: Empty argument!");
                }
                continue;
            }

            if (cont[i - 1] == '\t')
            {
                throw invalid_argument("Argument Error: Empty argument!");
            }

            arg_num++;
            if (arg_num > 3)
            {
                throw invalid_argument("Argument Error: Job description file has redundant arguments!");
            }
            if (arg_num < 3)
            {
                throw invalid_argument("Argument Error: Job description file has less arguments!");
            }
            arg_num = 0;
        }
        if (cont[i] == '\t')
        {
            arg_num++;
        }
    }
}

void Scheduler::splitToken(string str)
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
            job.ID = 1 + cnt/3; // assign job id
            Utils::arg_check(token);
            string buf(token);
            job.set_arr_time(stoi(buf));
        }
        else if (cnt % 3 == 2)
        {
            string buf(token);
            job.set_cmd(buf);
        }
        else
        {
            Utils::arg_check(token);
            string buf(token);
            job.set_dur_time(stoi(buf));
            job_queue.push_back(job);
        }
        token = strtok(NULL, "\t\n");
    }
}

void Scheduler::choosePolicy()
{
    auto cho = policyMap.find(policyInfo);
    if (cho == policyMap.end())
    {
        throw invalid_argument("Invalid policy!");
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
        throw invalid_argument("Invalid policy!");
        break;
    }
    //cout << *this;
}

void Scheduler::driveFIFO()
{
    // create a scheduler
    vector<Job> pq_arr;
    int len = get_job_num();
    int time = 0;
     
    for(int i = 0; i < len; ++i){
        pq_arr.emplace_back(job_queue[i]);  
    }
    
    sort(pq_arr.begin(), pq_arr.end()); 
    
    time = pq_arr[0].get_arr_time();
    
    for(int i = 0; i < len; ++i){
        auto it = pq_arr[i];
        time += it.get_dur_time();
        scheduler.emplace(it);
    }
    
    // display
    set_total_time(time);
    Display(scheduler);
    cout << endl;
    
    // execute
    int realtime = 0;
    int pid = 1;
   
    while(!stop_flag){
    
        cout << "now time: " << realtime << " s" << endl;    
        
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
                         
                         
                         signal(SIGALRM, signal_nonpreem);
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
        cout << "now time: " << realtime << " s" << endl;       
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
    // create a scheduler
    vector<Job> pq_arr;
    int len = get_job_num();
    int time = 0;
     
    for(int i = 0; i < len; ++i){
        pq_arr.emplace_back(job_queue[i]);  
    }
    
    sort(pq_arr.begin(), pq_arr.end(), [](const Job &a, const Job &b)->bool
    {   if(a.get_arr_time() == b.get_arr_time()){return a.get_dur_time() < b.get_dur_time();}
        else{return a.get_arr_time() < b.get_arr_time();}
    });
    
    time = pq_arr[0].get_arr_time();
    
    for(int i = 0; i < len; ++i){
        auto it = pq_arr[i];
        time += it.get_dur_time();
        scheduler.emplace(it);
    }
    
    // display
    set_total_time(time);
    Display(scheduler);
    cout << endl;
    
    // execute
    int realtime = 0;
    int pid = 1;
   
    while(!stop_flag){
    
        cout << "now time: " << realtime << " s" << endl;    
        
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
                         
                         
                         signal(SIGALRM, signal_nonpreem);
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
        cout << "now time: " << realtime << " s" << endl;       
        while (1) {
            int result = wait(NULL);
            if (result == -1) {
                if (errno == EINTR) { continue;}

                break;
            }
        }

    }
 
}

void Scheduler::driveRR(){
    
    // create a schedule
    priority_queue<Job,vector<Job>,greater<Job> > pq_arr;
    queue<Job> wait_q; 
    int len = get_job_num();
     
    for(int i = 0; i < len; ++i){
        pq_arr.emplace(job_queue[i]);  
    }
       
    int time = 0;
    int wait_for_empty = 0;
   
    while(true){
                                 
        while(!pq_arr.empty() && time == pq_arr.top().get_arr_time()){
            Job tmp = pq_arr.top();
            pq_arr.pop(); 
                
            if(tmp.get_dur_time() > QUANT){
                tmp.service_time_left = tmp.get_dur_time();
                scheduler.emplace(tmp);
                scheduler.back().set_dur_time(QUANT);
                tmp.set_dur_time(tmp.get_dur_time()-QUANT);
                wait_q.emplace(tmp);
                wait_for_empty += QUANT;
            }
            else{
                tmp.service_time_left = tmp.get_dur_time();
                scheduler.emplace(tmp);
                wait_for_empty += tmp.get_dur_time();
            }
                 
        }
        
        if(wait_for_empty == 0){
            if(wait_q.empty()){;}
            else{
                Job tmp2 = wait_q.front();
                wait_q.pop();
                tmp2.set_arr_time(time);
                if(tmp2.get_dur_time() > QUANT){
                    tmp2.service_time_left = tmp2.get_dur_time();
                    scheduler.emplace(tmp2);
                    scheduler.back().set_dur_time(QUANT);
                    tmp2.set_dur_time(tmp2.get_dur_time()-QUANT);
                    wait_q.emplace(tmp2);
                    wait_for_empty += QUANT;
                }
                else{
                    tmp2.service_time_left = tmp2.get_dur_time();
                    scheduler.emplace(tmp2);
                    wait_for_empty += tmp2.get_dur_time();
                }
            }
        }
        
        time++;
        if(wait_for_empty > 0){wait_for_empty--;}
        
        if(wait_q.empty() && pq_arr.empty()){break;}    
    }
    
        
    // display
    set_total_time(time);
    Display(scheduler);
    cout << endl;
    
    /*
    
    // display
    while(!scheduler.empty()){
        cout << scheduler.front().ID << " " << scheduler.front().get_arr_time() << " " << scheduler.front().service_time_left << endl;
        scheduler.pop();
    }
    
    */
    
    // execute
    int realtime = 0;
    int pid = 1;
        
    while(!stop_flag)
    { 
         cout << "now time: " << realtime << " s" << endl;    
         
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
                         
                         signal(SIGALRM, signal_preem);
                         alarm(it.get_dur_time());
                            
                         sleep(0.1);                
                         kill(monitor_map[it.ID], SIGCONT);    
                            
                         //cout << "allo_preem " << allow_preem<<endl;
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
        cout << "now time: " << realtime << " s" << endl;     
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
    ostream &operator<<(ostream &out, const Scheduler &sc)
    {
        out << "End" <<'\n';
        //out << "Totally " << sc.get_job_num() << " jobs\n";
        //out << '\n';
        //out << "-----------------------------------------------------------------------------------------------" << '\n';
        //out << "Gantt Chart" << '\n';
        //out << "-----------------------------------------------------------------------------------------------" << '\n';
        //out << "Time" << setfill(' ') << setw(8) << '|';
        //for (int i = 0; i <= sc.get_total_time() / 10; i++)
        //{
            //out << i;
            //out << setfill(' ') << setw(20);
        //}
        //out << '\n';
        //out << setfill(' ') << setw(12) << '|';
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
            //out << "Job " << i + 1 << setfill(' ') << setw(7) << '|';
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
        //out << "Mixed" << setfill(' ') << setw(7) << '|';
        //for(int i=0;i<sc.job_queue[0].get_arr_time();i++)
        //{
            //out<<"  ";
        //}
        //out << setfill(' ') << setw(sc.job_queue[0].get_arr_time() * 2);
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

void Scheduler::Display(queue<Job> q)
{
    cout<<"Totally " << get_job_num() << " jobs, Time using: " << get_total_time()<<" s\n";
    cout<<"======================================================================="<<'\n';
    cout<<"Gantt Chart"<<'\n';
    cout<<"======================================================================="<<'\n';
    cout << "Time" << setfill(' ') << setw(5) << "|";
     
    for (int i = 0; i <= get_total_time()/ 10; i++)
    {
        cout << i;
        cout << setfill(' ') << setw(20);
    }   
        
    cout << '\n';
    cout << setfill(' ') << setw(9) << '|';
    for (int i = 0; i <= get_total_time() / 10; i++)
    {
        for (int j = 0; j <= 9; j++){ cout << j << ' ';}
    }
    cout << '\n';
    
    int stopindex = 0;
    
    while(!q.empty())
    {
         auto it = q.front();
         q.pop();
         
         cout<<"Job " << it.ID <<setw(4) << "|";
         int arr_time = it.get_arr_time();
         int dur_time = it.get_dur_time();
         for(int i=0; i<arr_time; ++i){ cout << " " << ' ';}
         cout << "." << ' '; // arrive
         for(int i=arr_time + 1; i<=get_total_time(); ++i){ 
             if(i <= stopindex){cout << "." << ' ';}
             else{
                 cout << "#" << ' ';
                 dur_time--;
                 if(dur_time==0){stopindex = i; break;}
             }
         }
         for(int i=stopindex + 1; i<=get_total_time(); ++i){ cout << " " << ' ';}
         cout << endl;
    }
}


