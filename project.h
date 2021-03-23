#ifndef PROJECT_H
#define PROJECT_H
#include <iostream>
#include <vector>
#include <string>
#include <time.h>
#include <sys/times.h>

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
        void static cmd_TSTP(int sig);
        void static cmd_CONT(int sig);
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
    std::ostream& operator<< (std::ostream& out,const Monitor& m);
    
}
#endif
