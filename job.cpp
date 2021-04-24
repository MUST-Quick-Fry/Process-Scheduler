#include "project.h"
#include <iostream>
using namespace Project;

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

void Job::set_wait_time(int wait)
{
    this->wait_time=wait;
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

int Job::get_wait_time() const
{
    return this->wait_time;
}

Job Job::get_job()
{
    return *this;
}

bool Job::operator<(const Job & j) const
{
    return this->arrive < j.arrive;
}