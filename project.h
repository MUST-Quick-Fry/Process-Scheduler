#ifndef PROJECT_H
#define PROJECT_H
#include <iostream>
#include <vector>
#include <string>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <unordered_map>
#include <sys/times.h>
#include <algorithm>
#include <queue>
#include <unordered_map>

#define MAXJOBS 100000
namespace Project{
    class Job
    {
    private:
        std::string cmd;
        int arrive;
        int duration;
        int wait_time=0;
        
    public:
        int ID;
        int service_time_left;
        int has_exp_time;
        
        void set_wait_time(int wait);
        void set_cmd(std::string cmd);
        void set_arr_time(int arrive);
        void set_dur_time(int duration);
        std::string get_cmd() const;
        int get_wait_time() const;
        int get_arr_time() const;
        int get_dur_time() const;
        Job get_job();
        bool operator<(const Job& j) const;
        bool operator>(const Job& j) const;
        char status[30];
    };


    class Monitor
    {
    private:
        char **command;
        int process_pid=0;
        int self_pid;
        clock_t start,end;
        // /*
        // struct tms{
        //     clock_t tms_utime; /* user cpu time */
        //     clock_t tms_stime; /* system cpu time */
        //     clock_t tms_cutime; /* user cpu time of children */
        //     clock_t tms_cstime; /* system cpu time of children */
        // }
        // */
        struct tms t_start,t_end; 
        double user_time=0;
        double system_time=0;
        double time_elapsed=0;
        
        
        //get elapsed time of clock of system 
        double getTick(clock_t time);
        //spilt the space command 
        char** cmdSplit(std::string str);

    public:
        Monitor(Job j);
        Job job;
        void set_pid(int pid);
        void set_self_pid(int pid);
        void set_userTime(clock_t time);
        void set_systermTime(clock_t time);
        void set_timeElapsed(clock_t time);
        void set_all(int pid,clock_t use_time,clock_t system_time,clock_t time_elapsed);
        
        int get_self_pid() const;
        int get_pid() const;
        double get_userTime() const;
        double get_systemTime() const;
        double get_timeElapsed() const;
        
        void print_time();
        
        //void execute_command(char *command[]);
    };
    /*
        Summary:
            Monitor console output 
    */
    std::ostream& operator<< (std::ostream& out,const Monitor& m);
    
    class Scheduler
    {
    private:
        std::string policyInfo; 
        enum class Policy
        {
            FIFO,NONPREEMSJF,PREESJF,RR
        };
        const std::unordered_map<std::string,Policy> policyMap =
        {
            {"FIFO",Policy::FIFO},
            {"SJF1",Policy::NONPREEMSJF},
            {"SJF2",Policy::PREESJF},
            {"RR",Policy::RR}
        };
        // variable
        int job_num;
        int total_time;
        //Monitor monitor; 
              
        // method
        void normalizeCheck(std::string cont);
        void splitToken(std::string str);     
        void choosePolicy();
        //void schedulerDrive();
        
        //TBD
        void driveFIFO();
        void driveSJF1();
        void driveSJF2();
        void driveRR();
        
    public:
        Scheduler(char * f,char * p);
        ~Scheduler(){};
        void Display(std::queue<Job> q);
        void set_total_time(int now);
        int get_total_time() const;
        int get_job_num() const;
        std::vector<Monitor> monitor_vector;
        
    };
       
    //std::ostream& operator<< (std::ostream& out,const Scheduler& sc);
    
    // static variable & function
    static int QUANT = 2;  //RR scheduler with quantum set to 2 seconds
    static Job this_job;
    static bool stop_flag = false;
    static bool allow_preem = true;
    
    static std::vector<Job> job_queue; //job structure stored in vector
    static std::queue<Job> scheduler;
    static std::queue<Job> wait_queue;
    static std::unordered_map<int, int> m; //monitor and its child job
    static std::unordered_map<int ,int> monitor_map; // main process and monitor process
  
    
    static void sig_handler(int sig){
        std::cout << std::endl;
        if(sig==SIGTSTP){ 
            std::cout << "The Job is suspended ..." << std::endl;
            //std::cout << m[getpid()]<< std::endl;
            kill(m[getpid()], SIGTSTP);         
            signal(SIGTSTP, sig_handler);
        }
        else if(sig==SIGCONT){ 
            std::cout << "The Job resumes ... " << std::endl;
            //std::cout <<m[getpid()] << std::endl;
            kill(m[getpid()], SIGCONT);
            signal(SIGCONT, sig_handler);
        }
        else if(sig==SIGTERM){
            std::cout << "The Job terminate ... " << std::endl;
            //std::cout <<m[getpid()] << " child " <<std::endl;
            kill(m[getpid()], SIGTERM);
            //signal(SIGTERM, sig_handler);
        }
        
    }
    
    // handle RR problem
    static void signal_RR(int sig){
        
        if(scheduler.empty() && wait_queue.empty()){stop_flag = true;}
        
        if(this_job.service_time_left <= QUANT){
            kill(monitor_map[this_job.ID], SIGTERM);  
        }
        else{
            kill(monitor_map[this_job.ID], SIGTSTP);
        }
    
        if(!wait_queue.empty()){
            auto tmp = wait_queue.front();
            wait_queue.pop();

            kill(monitor_map[tmp.ID], SIGCONT);   
            this_job = tmp;

            signal(SIGALRM, signal_RR);
            alarm(tmp.get_dur_time());
        }
        else{allow_preem = true; }

    }
    
    // handle RR problem
    static void signal_preemSJF(int sig){
        
        if(scheduler.empty() && wait_queue.empty()){stop_flag = true;}
        
        if(this_job.service_time_left == 2){
            kill(monitor_map[this_job.ID], SIGTERM);  
        }
        else{
            kill(monitor_map[this_job.ID], SIGTSTP);
        }
    
        if(!wait_queue.empty()){
            auto tmp = wait_queue.front();
            wait_queue.pop();

            kill(monitor_map[tmp.ID], SIGCONT);   
            this_job = tmp;

            signal(SIGALRM, signal_preemSJF);
            alarm(tmp.get_dur_time());
        }
        else{allow_preem = true; }

    }
    
    // non-preemptive alarm signal
    static void signal_nonpreem(int sig){
        
        //std::cout << wait_queue.size() <<std::endl;
        
        if(scheduler.empty() && wait_queue.empty()){stop_flag = true;}
            
        kill(monitor_map[this_job.ID], SIGTERM);  
        
        if(!wait_queue.empty()){
            auto tmp = wait_queue.front();
            wait_queue.pop();

            kill(monitor_map[tmp.ID], SIGCONT);   
            this_job = tmp;

            signal(SIGALRM, signal_nonpreem);
            alarm(tmp.get_dur_time());
        }
        else{allow_preem = true; }

    }
    
}
#endif

#ifndef UTILS_H
#define UTILS_H
#include <string>

namespace Utils
{
    /*
        Summary:
	    Read file into std::string.
	 
        Param:
        char* fileName
        --The file to read<
	    
        Returns:
        A string containing the file content
    */
    
    std::string readFile(const char* fileName);
    bool isSpace(char ch);     
    bool arg_check(char* str);
}

#endif


