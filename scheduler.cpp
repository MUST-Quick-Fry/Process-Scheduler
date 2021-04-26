#include "project.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <algorithm>
#include <iomanip>
#include <queue>

using namespace Project;

//RR scheduler with quantum set to 2 seconds
constexpr int QUANT = 2;


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
    sort(this->job_queue.begin(), this->job_queue.end());
    std::cout<< "Totally "<<this->job_queue.size()<<" jobs are ready!" <<'\n';
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
        break;
    case Policy::RR:
        /*RR function interface*/
        std::cout << "hh"
                  << "\n";
        break;
    default:
        throw std::invalid_argument("Invalid policy!");
        break;
    }
    //std::cout << *this;
}

void Scheduler::driveFIFO()
{
    int now=0;
    int process = fork();
    if (process == 0){
        for (int i = 0; i < get_job_num(); i++)
        {
            Monitor *monitor;
          
            if(job_queue[i].get_dur_time()==-1){//duration time =-1 run indefinitely until it terminates
                job_queue[i].set_dur_time(monitor->job.get_dur_time());
            }
            if (i == 0)
            {
                sleep(job_queue[i].get_arr_time());
                now += job_queue[i].get_arr_time() + job_queue[i].get_dur_time();

            }
            monitor = new Monitor(job_queue[i]);
            if(job_queue[i].get_dur_time()==-1){
                job_queue[i].set_dur_time(monitor->job.get_dur_time());
            }
            if(i!=0)
            {
                if (now < job_queue[i].get_arr_time())
                {
                    sleep(job_queue[i].get_arr_time() - now);
                    now = job_queue[i].get_arr_time() + job_queue[i].get_dur_time();
                }
                else
                {
                    job_queue[i].set_wait_time(now-job_queue[i].get_arr_time());
                    job_queue[i].set_arr_time(now);  //arr_time change to exactly time mark for every job 
                    now += job_queue[i].get_dur_time();
                }
            }
            //monitor_vector.emplace_back(monitor);
            //std::cout<<monitor<<std::endl;
             
        }
        
        set_total_time(now);
        std::cout<<"now "<<now<<std::endl;
        std::cout<<*this;
    }
    else{
        waitpid(process, 0, 0);
    }
    
}

void Scheduler::driveSJF1()
{
    std::vector<Job> tmp;
    std::priority_queue<Job,std::vector<Job>,std::greater<Job>> pq;
    int now=0;
    int cnt=0;
    pq.emplace(job_queue[0]);
    int process=fork();
    
    if (process == 0){
        while(!pq.empty()){
            Monitor *monitor;
        
            auto it=pq.top();
            pq.pop();
            if (cnt == 0)
            {
                sleep(it.get_arr_time());
                now += it.get_arr_time() + it.get_dur_time();
            }
            monitor = new Monitor(it);
            if(it.get_dur_time()==-1){
                it.set_dur_time(monitor->job.get_dur_time());
            }
            if(cnt!=0)
            {
                if (now < it.get_arr_time())
                {
                    sleep(it.get_arr_time() - now);
                    now = it.get_arr_time() + it.get_dur_time();
                }
                else
                {
                    it.set_wait_time(now-it.get_arr_time());
                    it.set_arr_time(now); 
                    now += it.get_dur_time();
                }
            }
            tmp.emplace_back(it);
            for(int i=cnt+1;i<job_queue.size();i++){
                if(job_queue[i].get_arr_time()<=now){
                    pq.emplace(job_queue[i]);
                    cnt++;
                }
                else 
                    break;
            }
            if(pq.empty() && cnt<job_queue.size()-1){
                pq.emplace(job_queue[++cnt]);
            }
          
        }
        
        std::cout<<"size "<<pq.size()<<'\n';
        std::cout<<"cnt "<<cnt<<'\n';
        set_total_time(now);
        this->job_queue=tmp;
        std::cout<<*this;
             
    }
    else{ 
        waitpid(process, 0, 0);
    }
 
    //kill(process,SIGTERM);
    //kill(getpid(),SIGTERM);
}

namespace Project
{
    std::ostream &operator<<(std::ostream &out, const Scheduler &sc)
    {
        out << "Totally " << sc.get_job_num() << " jobs\n";
        out << '\n';
        out << "-----------------------------------------------------------------------------------------------" << '\n';
        out << "Gantt Chart" << '\n';
        out << "-----------------------------------------------------------------------------------------------" << '\n';
        out << "Time" << std::setfill(' ') << std::setw(8) << '|';
        for (int i = 0; i <= sc.get_total_time() / 10; i++)
        {
            out << i;
            out << std::setfill(' ') << std::setw(20);
        }
        out << '\n';
        out << std::setfill(' ') << std::setw(12) << '|';
        for (int i = 0; i <= sc.get_total_time() / 10; i++)
        {
            for (int j = 0; j <= 9; j++)
            {
                out << j << ' ';
            }
        }
        out << '\n';
        for (int i = 0; i < sc.get_job_num(); i++)
        {
            out << "Job " << i + 1 << std::setfill(' ') << std::setw(7) << '|';
            for(int j=0;j<sc.job_queue[i].get_arr_time()-sc.job_queue[i].get_wait_time()
            ;j++){
                out<<"  ";
            }
            for(int j=0;j<sc.job_queue[i].get_wait_time();j++)
            {
                out<<". ";
            }
            for (int j = 0; j < sc.job_queue[i].get_dur_time(); j++)
            {
                out << "# ";
            }
            out << '\n';
        }
        out << "Mixed" << std::setfill(' ') << std::setw(7) << '|';
        for(int i=0;i<sc.job_queue[0].get_arr_time();i++)
        {
            out<<"  ";
        }
        //out << std::setfill(' ') << std::setw(sc.job_queue[0].get_arr_time() * 2);
        for (int i = 0; i < sc.get_job_num(); i++)
        {
            for (int j = 0; j < sc.job_queue[i].get_dur_time(); j++)
            {
                out << i + 1 << ' ';
            }
        }
        out << '\n';
    }
}

void Scheduler::Display()
{
    // std::cout<<"Totally " << job_num << " jobs\n";
    // std::cout<<'\n';
    // std::cout<<"Gantt Chart"<<'\n';
    // std::cout<<"======================================"<<'\n';

    // for(auto it =job_queue.begin();it!=job_queue.end();it++)
    // {
    //     std::cout<<it->get_arr_time()<<" "<<it->get_cmd()<<" "<<it->get_dur_time()<<'\n';
    // }
}
