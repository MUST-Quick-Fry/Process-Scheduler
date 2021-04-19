#include "project.h"
#include <iostream>
using namespace Project;

void Job::set_pid(int pid)
{
    this->pid=pid;
}

void Job::set_state(int state)
{
    this->state=state;
}

void Job::set_cmd(std::string cmd)
{
    this->cmd=cmd;
}

void Job::set_arr_time(int arrive)
{
    this->arrive=arrive;
}

void Job::set_dur_time(int duration)
{
    this->duration=duration;
}

int Job::get_pid() const 
{
    return this->pid;
}

int Job::get_state() const
{
    return this->state;
}

std::string Job::get_cmd() const
{
    return this->cmd;
}

int Job::get_arr_time() const
{
    return this->arrive;
}

int Job::get_dur_time() const 
{
    return this->duration;
}

Job Job::get_job() const
{
    return *this;
}

bool Job::operator<(const Job & j) const
{
    return this->arrive < j.arrive;
}