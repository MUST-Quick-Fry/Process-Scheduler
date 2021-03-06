#include "project.h"
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <algorithm>
#include <iomanip>
#include <queue>
#include <math.h>

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

void Scheduler::find_dur_time(Job j, int& t){
    long tps=sysconf(_SC_CLK_TCK);

    tms start, end;
    clock_t s, e;
    s = times(&start);
    int pID = fork();
    if(pID == 0){ Monitor* monitor = new Monitor(j); exit(0);}
    else{
        waitpid(pID, 0, 0);
        e = times(&end); 
        double realtime = (double)(e-s)/tps;
        
        if(round(realtime)==0){t = 1;}
        else{t= round(realtime);}
    }
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

    // clear -1     
    for(int i = 0; i < len; ++i){
        if(job_queue[i].get_dur_time()==-1){ 
            int t = 0;
            find_dur_time(job_queue[i], t);
            //cout << t << endl;
            job_queue[i].set_dur_time(t);
        }

        pq_arr.emplace_back(job_queue[i]);  
    }
    
    sort(pq_arr.begin(), pq_arr.end()); 
    
    time = pq_arr[0].get_arr_time();
    
    for(int i = 0; i < len; ++i){
        auto it = pq_arr[i];
        time += it.get_dur_time();
        scheduler.emplace(it);
    }
   
       
    // copy
    set_total_time(time);   
    real_job_queue = scheduler;
    cout << endl;
    cout << endl;
    cout << endl;
    
    // execute
    cout << "==================================================="<< endl;
    cout << "Now start the Scheduler !!!"<< endl;
    cout << "==================================================="<< endl;
    
    int realtime = 0;
    int pid = 1;
   
    while(!stop_flag){
     
        cout << "now time: " << realtime << " s" << endl;    
        
        while(!scheduler.empty() && realtime >= scheduler.front().get_arr_time()){
            auto it = scheduler.front();
            scheduler.pop();
                        
            if(monitor_map[it.ID]==0){
                pid = fork();
                if(pid == 0){
                    Monitor* monitor = new Monitor(it);                       
                    cout << "Terminate the Parent Process: " << getpid() <<endl;     
                    
                    //kill(getpid(), SIGKILL);
                    exit(0);
  
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
 
                     }
                     else{
                         wait_queue.emplace(it);
                         kill(monitor_map[it.ID], SIGTSTP);
                         std::cout<<"Child Process Suspend" << std::endl;
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
        sleep(2);
        cout << endl;
        cout << endl;
        
        // draw Gantt diagram
        Display(real_job_queue);

    }
    
}

void Scheduler::driveSJF1()
{
    // create a scheduler
    priority_queue<Job,vector<Job>,greater<Job> > wait_q; 
    queue<Job> pq_arr;
    int len = get_job_num();
    int time = 0;

    // clear -1        
    for(int i = 0; i < len; ++i){
        if(job_queue[i].get_dur_time()==-1){ 
            int t = 0;
            find_dur_time(job_queue[i], t);
            //cout << t << endl;
            job_queue[i].set_dur_time(t);
        }
    }
        
    sort(job_queue.begin(), job_queue.end(), [](const Job &a, const Job &b)->bool
    {   if(a.get_arr_time() == b.get_arr_time()){return a.get_dur_time() < b.get_dur_time();}
        else{return a.get_arr_time() < b.get_arr_time();}
    });
    
    for(int i = 0; i < len; ++i){
        pq_arr.emplace(job_queue[i]);  
    }
    
    time = pq_arr.front().get_arr_time();
    
    while(true){
    
        if(!wait_q.empty()){
            auto it = wait_q.top();
            wait_q.pop();
            time += it.get_dur_time();
            scheduler.emplace(it);    
        }
    
        if(!pq_arr.empty()){
            auto it = pq_arr.front();
            pq_arr.pop();
            time += it.get_dur_time();
            scheduler.emplace(it);
        
            while(!pq_arr.empty() && pq_arr.front().get_arr_time()<=time){
                wait_q.emplace(pq_arr.front());
                pq_arr.pop();
            }
        }
        if(pq_arr.empty() && wait_q.empty()){break;}
    }
    
    // copy
    set_total_time(time);
    real_job_queue = scheduler;
    cout << endl;
    cout << endl;
    cout << endl;
    
    // execute
    cout << "==================================================="<< endl;
    cout << "Now start the Scheduler !!!"<< endl;
    cout << "==================================================="<< endl;
    
    int realtime = 0;
    int pid = 1;
   
    while(!stop_flag){
     
        cout << "now time: " << realtime << " s" << endl;    
        
        while(!scheduler.empty() && realtime >= scheduler.front().get_arr_time()){
            auto it = scheduler.front();
            scheduler.pop();
                        
            if(monitor_map[it.ID]==0){
                pid = fork();
                if(pid == 0){
                    Monitor* monitor = new Monitor(it);                       
                    cout << "Terminate the Parent Process: " << getpid() <<endl;     
                    
                    //kill(getpid(), SIGKILL);
                    exit(0);
  
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
 
                     }
                     else{
                         wait_queue.emplace(it);
                         kill(monitor_map[it.ID], SIGTSTP);
                         std::cout<<"Child Process Suspend" << std::endl;
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
        sleep(2);
        cout << endl;
        cout << endl;
        
        // draw Gantt diagram
        Display(real_job_queue);

    }

}

void Scheduler::driveRR(){
    
    // create a schedule
    queue<Job> pq_arr;
    queue<Job> wait_q; 
    int len = get_job_num();
    
    // clear -1
    for(int i = 0; i < len; ++i){
        if(job_queue[i].get_dur_time()==-1){ 
            int t = 0;
            find_dur_time(job_queue[i], t);
            //cout << t << endl;
            job_queue[i].set_dur_time(t);
        }
    }
     
    sort(job_queue.begin(), job_queue.end());
     
    for(int i = 0; i < len; ++i){
        pq_arr.emplace(job_queue[i]);  
    }
       
    int time = 0;
    int wait_for_empty = 0;
   
    while(true){
                                 
        while(!pq_arr.empty() && time == pq_arr.front().get_arr_time()){
            Job tmp = pq_arr.front();
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
    
        
    // copy
    set_total_time(time);
    real_job_queue = scheduler;
    cout << endl;
    cout << endl;
    cout << endl;
    
    // execute
    cout << "==================================================="<< endl;
    cout << "Now start the Scheduler !!!"<< endl;
    cout << "==================================================="<< endl;
    
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
                    cout << "Terminate the Parent Process: " << getpid() <<endl;     
                    
                    //kill(getpid(), SIGKILL);
                    exit(0);
  
                }
                else{ 
                    monitor_map[it.ID]=pid;
                }
            }
            
            
            if(pid !=0){     
                                           
                     if(allow_preem){                 
                         allow_preem = false;                                  
                         this_job = it;
                         
                         kill(monitor_map[it.ID], SIGCONT);
                         signal(SIGALRM, signal_RR);
                         alarm(it.get_dur_time());
 
                     }
                     else{
                         wait_queue.emplace(it);
                         kill(monitor_map[it.ID], SIGTSTP);
                         std::cout<<"Child Process Suspend" << std::endl;
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
        sleep(2);
        cout << endl;
        cout << endl;
        
        // draw Gantt diagram
        Display(real_job_queue);

    }
        
}

void Scheduler::driveSJF2(){

    // create a schedule
    queue<Job> pq_arr; 
    priority_queue<Job,vector<Job>,greater<Job> > wait_q; 
    int len = get_job_num();

    // clear -1
    for(int i = 0; i < len; ++i){
        if(job_queue[i].get_dur_time()==-1){ 
            int t = 0;
            find_dur_time(job_queue[i], t);
            //cout << t << endl;
            job_queue[i].set_dur_time(t);
        }
    }
         
    sort(job_queue.begin(), job_queue.end(), [](const Job &a, const Job &b)->bool
    {   if(a.get_arr_time() == b.get_arr_time()){return a.get_dur_time() < b.get_dur_time();}
        else{return a.get_arr_time() < b.get_arr_time();}
    });
         
    for(int i = 0; i < len; ++i){
        pq_arr.emplace(job_queue[i]);  
    }
       
      
    int time = 0;
    Job con;
    con.ID = 0;
    unordered_map<int, int> jobmap;
    Job lastjob = con;
    
    while(true){                                          
        while(!pq_arr.empty() && time == pq_arr.front().get_arr_time()){
            Job tmp = pq_arr.front();
            pq_arr.pop(); 
            
            if(!wait_q.empty() && tmp.get_dur_time() < wait_q.top().get_dur_time() && wait_q.top().ID == lastjob.ID)
                wait_q.pop();
            wait_q.emplace(tmp);         
        }
        
   
        if(!wait_q.empty()){
            
            Job it = wait_q.top();
            wait_q.pop();
            
            if(lastjob.ID == it.ID || lastjob.ID == 0){
            
                if(jobmap[it.ID] == 0){
                    scheduler.emplace(it);
                    jobmap[it.ID] = 1;
                }
            
                it.has_exp_time++;
                it.set_dur_time(it.get_dur_time()-1);
                           
                if(it.get_dur_time() == 0){
                    scheduler.back().set_dur_time(it.has_exp_time);
                    scheduler.back().service_time_left = 2;
                    //cout << scheduler.back().ID << " arr " << scheduler.back().get_arr_time() << " dur "<< scheduler.back().get_dur_time()<< endl;
                    jobmap[it.ID] = 0;
                    lastjob = con;
                }
                else{ 
                    if(wait_q.empty() || it.get_dur_time() < wait_q.top().get_dur_time())
                        wait_q.emplace(it); 
                        
                    lastjob = it;
                }
                
            }
            else{
                scheduler.back().set_dur_time(lastjob.has_exp_time);
                //cout << scheduler.back().ID << " arr " << scheduler.back().get_arr_time() << " dur "<< scheduler.back().get_dur_time()<< endl;
                jobmap[lastjob.ID] = 0;
                lastjob.has_exp_time = 0;
                lastjob.set_arr_time(time);
                wait_q.emplace(lastjob);
                
             
                if(jobmap[it.ID] == 0){
                    scheduler.emplace(it);
                    jobmap[it.ID] = 1;
                }
            
                it.has_exp_time++;
                it.set_dur_time(it.get_dur_time()-1);
                
                
                if(it.get_dur_time() == 0){
                    scheduler.back().set_dur_time(it.has_exp_time);
                    scheduler.back().service_time_left = 2;
                    //cout << scheduler.back().ID << " arr " << scheduler.back().get_arr_time() << " dur "<< scheduler.back().get_dur_time()<< endl;
                    jobmap[it.ID] = 0;
                    lastjob = con;
                }
                else{ 
                    if(wait_q.empty() || it.get_dur_time() < wait_q.top().get_dur_time())
                        wait_q.emplace(it); 
                        
                    lastjob = it;
                }            
                
            }
                
                
        }
        
        time++;
        //cout << "now is " << time << " " << wait_q.size() << endl;
        if(wait_q.empty() && pq_arr.empty()){break;}    
    }
    
       
    // copy
    set_total_time(time);
    real_job_queue = scheduler;
    cout << endl;
    cout << endl;
    cout << endl;
    
    // execute
    cout << "==================================================="<< endl;
    cout << "Now start the Scheduler !!!"<< endl;
    cout << "==================================================="<< endl;
    
    int realtime = 0;
    int pid = 1;
   
    while(!stop_flag){
     
        cout << "now time: " << realtime << " s" << endl;    
        
        while(!scheduler.empty() && realtime >= scheduler.front().get_arr_time()){
            auto it = scheduler.front();
            scheduler.pop();
                        
            if(monitor_map[it.ID]==0){
                pid = fork();
                if(pid == 0){
                    Monitor* monitor = new Monitor(it);                       
                    cout << "Terminate the Parent Process: " << getpid() <<endl;     
                    
                    //kill(getpid(), SIGKILL);
                    exit(0);
  
                }
                else{ 
                    monitor_map[it.ID]=pid;
                }
            }
            
            
            if(pid !=0){     
                                           
                     if(allow_preem){                 
                         allow_preem = false;                                  
                         this_job = it;
                         
                         kill(monitor_map[it.ID], SIGCONT);
                         signal(SIGALRM, signal_preemSJF);
                         alarm(it.get_dur_time());
 
                     }
                     else{
                         wait_queue.emplace(it);
                         kill(monitor_map[it.ID], SIGTSTP);
                         std::cout<<"Child Process Suspend" << std::endl;
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
        sleep(2);
        cout << endl;
        cout << endl;
        
        // draw Gantt diagram
        Display(real_job_queue);

    }
    
}

namespace Project
{
    ostream &operator<<(ostream &out, const Scheduler &sc)
    {
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


