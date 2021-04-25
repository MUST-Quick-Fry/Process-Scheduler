#ifndef PROJECT_H
#define PROJECT_H
#include <iostream>
#include <vector>
#include <string>
#include <time.h>
#include <unordered_map>
#include <sys/times.h>
#include <algorithm>

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
        Monitor(char *command[]);
        Monitor(Job j);
        Monitor();
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
        
        void execute_command(char *command[]);
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
        void Display();
        void set_total_time(int now);

        int get_total_time() const;
        int get_job_num() const;
        std::vector<Monitor> monitor_vector;
        std::vector<Job> job_queue;//job structure stored in vector
    };
    
    std::ostream& operator<< (std::ostream& out,const Scheduler& sc);
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
    
} // namespace Utils

#endif
