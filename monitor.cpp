#include "project.h"
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <stdexcept>
#include <stdlib.h>
using namespace Project;
using namespace std;

double Monitor::getTick(clock_t time){
    long tps=sysconf(_SC_CLK_TCK);
    return (double)time/tps;
}

//string to char**
char** Monitor::cmdSplit(std::string str)
{
    //string to char*
    std::vector<std::string> buf;
    std::string tmp;
    std::stringstream ss;
    ss<<str;
    
    while(ss>>tmp)
    {
        buf.push_back(tmp + "");
    }
     
    //char* to char**
    char** pList=new char*[buf.size()];
    for(int i=0; i!=buf.size();i++)
    {
        pList[i]=const_cast<char*>(buf[i].c_str());
    }
    //cout<<pList<<endl;

    //delete[] pList;
    return pList;
    
}

Monitor::Monitor(Job j):job(j)
{
    //cout<<j.get_cmd()<<endl;
    
    char** command = cmdSplit(j.get_cmd());
    
    int process;
    int status;
     
    set_self_pid(getpid());
    start = times(&t_start);
    process = fork();
    
    if(process < 0){
	// error
        status = -1;
    }
    else if(process == 0){ 
     
        //cout << "child suspend!!!!!!!!!!!!" << endl;
        //kill(getpid(), SIGTSTP); // suspend child
        
        // child process
        cout << "=================================================" << endl;
        cout << "Job Process Start ! " << getpid() << endl;
        cout << endl;
        cout<<"command "<<command[0]<<endl;
        execvp(command[0],command);
        throw invalid_argument("Fail to execute Child Process!\n");
    }
    else{
        
        set_pid(process);
        
        if(m[getpid()]==0){m[getpid()] = process;}
                
                   
        signal(SIGTSTP, sig_handler);
        signal(SIGCONT, sig_handler);
        signal(SIGTERM, sig_handler);
        
        //cout << process << " " << getpid() << endl;
                            
        
        kill(process, SIGTSTP);
        std::cout<<"Child Process Suspend" << std::endl;

	// parent process
        waitpid(process,&status,0);
        std::cout<<"Child Process Terminate"<<std::endl;
                 
        //std::cout << getpid() << " " << m[getpid()] << std::endl;

        //signal(SIGTERM, sig_handler);
        //signal(SIGCHLD, sig_handler);
        
        //if(this->job.get_dur_time()>=0){
          //  alarm(this->job.get_dur_time());
        //}
        
        // calculate time
        end = times(&t_end);  
        set_timeElapsed(end-start);
        set_userTime(t_end.tms_cutime);
        set_systermTime(t_end.tms_cstime);
        
        // output 
        cout << endl;        
        cout << endl;
        print_time();
        
    }
    
    
    //if(this->job.get_dur_time()>=0){
    //    double remain_time=this->job.get_dur_time()-this->get_timeElapsed();
    //    if(remain_time>0)
    //    {
    //        sleep(remain_time);
    //    }         
    //}       
    //else{
    //   this->job.set_dur_time(this->get_timeElapsed());
    //}
    
    
}

/*
Monitor::Monitor(char *command[]):command(command)
{
    execute_command(command);
}
*/

void Monitor::set_self_pid(int pid){
    this->self_pid=pid;
}
void Monitor::set_pid(int pid){
    this->process_pid=pid;
}
void Monitor::set_systermTime(clock_t time){
    this->system_time=getTick(time);
}
void Monitor::set_userTime(clock_t time){
    this->user_time=getTick(time);
}
void Monitor::set_timeElapsed(clock_t time){
    this->time_elapsed=getTick(time);
}

void Monitor::set_all(int pid,clock_t use_time,clock_t system_time,clock_t time_elapsed){

}

int Monitor::get_pid() const{
    return this->process_pid;
}
int Monitor::get_self_pid()const{
    return this->self_pid;
}
double Monitor::get_userTime() const{
    return this->user_time;
}
double Monitor::get_systemTime() const{
    return this->system_time;
}
double Monitor::get_timeElapsed() const{
    return this->time_elapsed;
}

void Monitor::print_time(){
    cout<<"Job Process "<<setw(6)<<get_pid()<<" : Time Elapsed: "<<get_timeElapsed()<<'\n';
    cout<<setfill(' ')<<setw(35)<<"User Time   : "<<get_userTime()<<'\n';
    cout<<setfill(' ')<<setw(35)<<"System Time : "<<get_systemTime()<<'\n';
    cout << "=================================================" << '\n';
    cout << '\n';
}

