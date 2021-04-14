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
        execvp(command[0],command);
        throw invalid_argument("Fail to execute Child Process!\n");
    }
    else{
	// parent process
        int status;
        set_pid(process);
        sig_pid = get_pid();
        signal(SIGTSTP, sig_handler);
        signal(SIGCONT, sig_handler);
        signal(SIGTERM, sig_handler);
        waitpid(sig_pid,&status,0);

	// quit loop after child process terminated!
    }
    
    // calculate time
    end = times(&t_end);  
    set_timeElapsed(end-start);
    set_userTime(t_end.tms_cutime);
    set_systermTime(t_end.tms_cstime);
    
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
        return out;
    }
}
