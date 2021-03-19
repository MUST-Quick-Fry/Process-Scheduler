#ifndef PROJECT_H
#define PROJECT_H
#include<iostream>
#include<vector>
#include<string>

namespace Project{
    class Monitor
    {
    private:
        char **command;
        int process_pid=0;
        double user_time=0;
        double system_time=0;
        double time_elapsed=0;
    public:
        Monitor(char *command[]):command(command){
            execute_command(command);
        };
        void set_pid(int pid);
        void set_userTime(double time);
        void set_systermTime(double time);
        void set_timeElapsed(double time);
        void set_all(double pid,double use_time,double system_time,double time_elasped);
        void execute_command(char *command[]);
        int get_pid() const;
        double get_userTime() const;
        double get_systemTime() const;
        double get_timeElapsed() const;
    };
    std::ostream& operator<< (std::ostream& out,const Monitor& m);
    
}
#endif
