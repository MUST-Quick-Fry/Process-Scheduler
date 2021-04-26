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
using namespace Project;
using namespace std;

static int sig_pid = 0; // child process pid

static void sig_handler(int sig){
    cout << endl;
    if(sig==SIGTSTP){
        cout << "The Job is suspended ..." << endl;
        kill(sig_pid, SIGTSTP);            
        signal(SIGTSTP, sig_handler);
    }
    else if(sig==SIGCONT){
        cout << "The Job resumes ... " << endl;
        kill(sig_pid, SIGCONT);
        signal(SIGCONT, sig_handler);
    }
    else if(sig==SIGTERM){
        cout << "The Job has been terminated !!" << endl;
        kill(sig_pid, SIGTERM);
        signal(SIGTERM, sig_handler);
    }
    else if(sig==SIGALRM){
        cout<<"Time out"<<endl;
        kill(sig_pid,SIGALRM);
        //signal(SIGALRM,sig_handler);
    }
}

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
    execute_command(command);
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

void Monitor::execute_command(char *command[]){
    int process;
    int status;
    
    cout << "=================================================" << endl;
    cout << "Job Process Start !" << endl;
    cout << endl;

    start = times(&t_start);
    process = fork();

    if(process < 0){
	// error
        status = -1;
    }
    else if(process == 0){ 
        // child process
        cout<<"command "<<command[0]<<endl;
        execvp(command[0],command);
        throw invalid_argument("Fail to execute Child Process!\n");
    }
    else{
	// parent process
        int status;
        set_pid(process);
        sig_pid = process;
        set_self_pid(getpid());
        signal(SIGALRM,sig_handler);
        signal(SIGTSTP, sig_handler);
        signal(SIGCONT, sig_handler);
        signal(SIGTERM, sig_handler);
        if(this->job.get_dur_time()>=0){
            alarm(this->job.get_dur_time());
        }
        waitpid(sig_pid,&status,0);

	// quit loop after child process terminated!
    }
    
    // calculate time
    end = times(&t_end);  
    set_timeElapsed(end-start);
    set_userTime(t_end.tms_cutime);
    set_systermTime(t_end.tms_cstime);
    if(this->job.get_dur_time()>=0){
        double remain_time=this->job.get_dur_time()-this->get_timeElapsed();
        if(remain_time>0)
        {
            sleep(remain_time);
        }         
    }       
    else{
        this->job.set_dur_time(this->get_timeElapsed());
    }
    // output 
    cout << endl;  
    cout << "Terminate the Parent Process !" << endl;
    cout << endl;
    cout<<*this;
}

namespace Project{
    ostream& operator<<(ostream& out,const Monitor& m){
        out<<"Job Process "<<setw(6)<<m.get_pid()<<" : Time Elapsed: "<<m.get_timeElapsed()<<'\n';
        out<<setfill(' ')<<setw(35)<<"User Time   : "<<m.get_userTime()<<'\n';
        out<<setfill(' ')<<setw(35)<<"System Time : "<<m.get_systemTime()<<'\n';
        out << "=================================================" << '\n';
        out << '\n';
        return out;
    }
}
