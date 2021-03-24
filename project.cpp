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

static int sig_pid = 0;

static int quit = 0;

bool flag=false;

static void cmd_TSTP(int sig){
    std::cout << std::endl;
    flag=true;
    std::cout << "The Job is suspended ..." << std::endl;
    kill(sig_pid, SIGTSTP);
}

static void cmd_CONT(int sig){
    std::cout << std::endl;
    std::cout << "The Job resumes ... " << std::endl;
    kill(sig_pid, SIGCONT);
}

static void cmd_KILL(int sig){
    int status;
    std::cout << std::endl;
    std::cout << "Terminate the Child process !!" << std::endl;
    kill(sig_pid, SIGTERM);
    quit = 1;
}

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
    
    start = times(&t_start);
    process = fork();

    if(process < 0){
        status = -1;
    }
    else if(process == 0){ 
        //signal(SIGINT, SIG_DFL); //Default handle every child process
        execvp(command[0],command);
        throw invalid_argument("Fail to execute Child Process!\n");
    }
    else{
        set_pid(process);
        sig_pid = get_pid();
        
	    while(!quit){
	    // listen to cmd signal	
            signal(SIGTSTP, cmd_TSTP);
            signal(SIGCONT, cmd_CONT);
            signal(SIGTERM, cmd_KILL);
        }

	// quit loop if the child process has been terminated!
    }
    
    // calculate time
    end = times(&t_end);  
    set_timeElapsed(end-start);
    set_userTime(t_end.tms_cutime);
    set_systermTime(t_end.tms_cstime);
    cout << endl; 
    sleep(3);
    cout << "Terminate the Parent process !" << endl;
    cout<<*this;
}

namespace Project{
    ostream& operator<<(ostream& out,const Monitor& m){
        out<<"Process "<<setw(6)<<m.get_pid()<<" : time elapsed: "<<m.get_timeElapsed()<<'\n';
        out<<setfill(' ')<<setw(31)<<"user time   : "<<m.get_userTime()<<'\n';
        out<<setfill(' ')<<setw(31)<<"system time : "<<m.get_systemTime()<<'\n';
        return out;
    }
}
