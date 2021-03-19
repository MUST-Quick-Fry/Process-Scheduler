#include "project.h"
#include <iostream>
#include <unistd.h>
#include <iomanip>
#include <cstring>
using namespace Project;
using namespace std;
void Monitor::set_pid(int pid){
    this->process_pid=pid;
}
void Monitor::set_systermTime(double time){
    this->system_time=time;
}
void Monitor::set_userTime(double time){
    this->user_time=time;
}
void Monitor::set_timeElapsed(double time){
    this->time_elapsed=time;
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
    process=fork();
    if(process==0){
        execvp("ls",command);
        cout<<"fail";
    }
    set_pid(process);
}
namespace Project{
    ostream& operator<<(ostream& out,const Monitor& m){
        out<<"Process "<<setw(6)<<m.get_pid()<<" : time elaspsed: "<<m.get_timeElapsed()<<'\n';
        out<<setfill(' ')<<setw(30)<<"user time   :"<<m.get_userTime()<<'\n';
        out<<setfill(' ')<<setw(30)<<"system time :"<<m.get_systemTime()<<'\n';
        return out;
    }
}