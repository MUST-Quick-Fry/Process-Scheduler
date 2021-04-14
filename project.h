#ifndef PROJECT_H
#define PROJECT_H
#include <iostream>
#include <vector>
#include <string>
#include <climits>
#include <time.h>
#include <sys/times.h>

#define MAXJOBS 100000
namespace Project{
    class Monitor
    {
    private:
        char **command;
        int process_pid=0;
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

        double getTick(clock_t time);

    public:
        Monitor(char *command[]):command(command){
            execute_command(command);
        };
        void set_pid(int pid);
        void set_userTime(clock_t time);
        void set_systermTime(clock_t time);
        void set_timeElapsed(clock_t time);
        void set_all(int pid,clock_t use_time,clock_t system_time,clock_t time_elapsed);
        void execute_command(char *command[]);
        int get_pid() const;
        double get_userTime() const;
        double get_systemTime() const;
        double get_timeElapsed() const;
    };
    /*
        Summary:
            Monitor console output 
    */
    std::ostream& operator<< (std::ostream& out,const Monitor& m);
    
    class Scheduler
    {
    private:
        char* policyInfo; 
        std::string fileInfo;
        enum class Policy
        {
            FIFO,NONPREEMSJF,PREESJF,RR
        };
        int num; //number of jobs
        
        //job info from description file
        struct Jobs
        {
            int pid;
            int state;
            std::string cmd;
            int arrive;
            int duration;
        }jobs[MAXJOBS];
        // std::vector<int> arr_time;  
        // std::vector<std::string> jobCommand;
        // std::vector<int> dur_time;
        
        std::vector<Jobs> v_jobs;//job structure stored in vector
        void fileToken(std::string str);
        //TBD
        
    public:
        Scheduler(char * f);
        virtual ~Scheduler(){};
        void myPrint();
    
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
} // namespace Utils

#endif