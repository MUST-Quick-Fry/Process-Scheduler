#include "project.h"
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <iomanip>
#include <cstring>
#include <stdexcept>
using namespace Project;
using namespace std;

double Monitor::getTick(clock_t time){
    long tps=sysconf(_SC_CLK_TCK);
    return (double)time/tps;
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

    start=times(&t_start);
    process=fork();
    if(process<0){
        status=-1;
    }
    else if(process==0){    
        signal(SIGINT, SIG_DFL); //Default handle every child process
        execvp(command[0],command);
        throw invalid_argument("Fail Child Process\n");//cout<<"fail";
    }
    else{
        while(waitpid(process,&status,0)<0){ //also can implemented by signal() SIGCHILD  
            if(errno!=EINTR){
                status=-1;
                break;
            }
        }
    }
    end=times(&t_end);
    set_pid(process);
    set_timeElapsed(end-start);
    set_userTime(t_end.tms_cutime);
    set_systermTime(t_end.tms_cstime);

    //kill process
    //kill(getpid(),SIGTERM);
    //kill(process,SIGTERM);
}
namespace Project{
    ostream& operator<<(ostream& out,const Monitor& m){
        out<<"Process "<<setw(6)<<m.get_pid()<<" : time elaspsed: "<<m.get_timeElapsed()<<'\n';
        out<<setfill(' ')<<setw(30)<<"user time   :"<<m.get_userTime()<<'\n';
        out<<setfill(' ')<<setw(30)<<"system time :"<<m.get_systemTime()<<'\n';
        return out;
    }
}