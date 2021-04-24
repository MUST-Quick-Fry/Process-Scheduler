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
    std::cout<<this->job_queue.size()<<'\n';
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
    for (int i = 0; i < get_job_num(); i++)
    {
        Monitor *monitor;
        if (process == 0)
        {
            monitor = new Monitor(job_queue[i]);
            if(job_queue[i].get_dur_time()==-1){
                job_queue[i].set_dur_time(monitor->job.get_dur_time());
            }
            if (i == 0)
            {
                sleep(job_queue[i].get_arr_time());
                now += job_queue[i].get_arr_time() + job_queue[i].get_dur_time();

            }
            else
            {
                if (now < job_queue[i].get_arr_time())
                {
                    sleep(job_queue[i].get_arr_time() - now);
                    now = job_queue[i].get_arr_time() + job_queue[i].get_dur_time();
                }
                else
                {
                    job_queue[i].set_wait_time(now-job_queue[i].get_arr_time());
                    job_queue[i].set_arr_time(now); 
                    now += job_queue[i].get_dur_time();
                }
            }
            //monitor_vector.emplace_back(monitor);
            //std::cout<<monitor<<std::endl;
        }
        else
        {
            waitpid(monitor->get_self_pid(), 0, 0);
        }
    }
    if (!process)
    {
        //waitpid(process,0,0);
        set_total_time(now);
        std::cout<<"now "<<now<<std::endl;
        std::cout<<*this;
    }
    waitpid(process,0,0);
}

namespace Project
{
    std::ostream &operator<<(std::ostream &out, const Scheduler &sc)
    {
        out << "Totally " << sc.get_job_num() << " jobs\n";
        out << '\n';
        out << "Gantt Chart" << '\n';
        out << "=============" << '\n';
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
            // for (int j = 0,k=1; 
            // j < sc.job_queue[k].get_arr_time()-sc.job_queue[k-1].get_dur_time()-sc.job_queue[k].get_wait_time(); j++,k++)
            // {
            //     out << "  ";
            // }
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
