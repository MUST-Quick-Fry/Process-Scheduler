# Process-Scheduler
Project of CO004 Operating Systems

# Author
    CHEN YUXUAN 1809853J-I011-0011  
    HE PEILIN 1809853U-I011-0078  
    WANG YUYANG 1809853Z-I011-0045  

# Team cooperation
Tasks completed by each person are documented below:

|Name|Task|
|----|----------|
|CHEN YUXUAN|Design the main framework , part of implementation of algorithms|
|WANG YUYANG|Implementation of 4 scheduling algorithms, signal-handler and normalization check of job file|
|HE PEILIN|Implementation of monitor time control, pretreat job description file and part of design the framework, report writing|

# Introduction 
In this project, we are proposed to design a simple process scheduler which be implemented with the function of
jobs analysis with different parameters in `.txt` file and involution of process creation, suspension, as well as termination. 

The programme includes three classes: **Scheduler**, **Monitor** and **job**.
We realize 4 scheduling algorithms :
- FIFO
- Non-preemptive SJF
- Preemptive SJF
- RR 
# Job description file
The standard file format :
>*[arrive time]* `\t` *[Job]* `\t` *[duration time]* `\n`

if you can not open file with invalid error, but it is in standard format, you could use `vim` or `vi` to change the file format as below : 
```code
set ff=unix
```

# How to compile and execute
## Environment
|Attribute|Content|
|-------------------------|-------------------------------|
|Operation System|ubuntu-20.04.1-desktop-amd64|
|Programming Language| C++11 |
|Compiler|g++ version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)|
## Makefile
You can easily compile the programme by `./Makefile` provided in root directory of this project using the command below :
```code
make 
```
To debug with programme using command below : 
```code
make ver=debug
```
To compile programme with optimization using command below :
```code
make ver=release
```
And delete `*.d` ,`*.o` files and the executable file using command below :
```code
make clean
```
How to compile two independent Phases :  
- **Phase 1** (**Monitor**) : change the `target` in `./Makefile` as 
>```text
> target:=job monitor monitor_main
> EXEC:=Monitor

- **Phase 2** (**Scheduler**) : change the `target` in `./Makefile` as 
>```text
> target:=utils job monitor scheduler scheduler_main
> EXEC:=Scheduler

# Usage
For **Phase 1**, with parameters :
```text
./Monitor [command string]
```
For **Phase 2**, with parameters :
```text
./Scheduler [filename] [policy] 
```
> The second argument `[policy]` only allows 4 choices in upper-case :
> **FIFO SJF1 SJF2 RR** 
> > **SJF1** : non-preemptive,
> > **SJF2** : preemptive 
# Gantt Chart
The Gantt Chart followed by job description file `./job4` as example.  
## FIFO
``` text
Totally 8 jobs, Time using: 23 s
=======================================================================
Gantt Chart
=======================================================================
Time    |0                   1                   2
        |0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9
Job 1   |. # # # # # #
Job 3   |  . . . . . . # # # #
Job 2   |    . . . . . . . . . # # # # #
Job 5   |      . . . . . . . . . . . . . # #
Job 6   |      . . . . . . . . . . . . . . . #
Job 4   |        . . . . . . . . . . . . . . . #
Job 7   |            . . . . . . . . . . . . . . # #
Job 8   |            . . . . . . . . . . . . . . . . # #

```
## Non-preemptive SJF
``` text
Totally 8 jobs, Time using: 23 s
=======================================================================
Gantt Chart
=======================================================================
Time    |0                   1                   2
        |0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9
Job 1   |. # # # # # #
Job 6   |      . . . . #
Job 4   |        . . . . #
Job 7   |            . . . # #
Job 5   |      . . . . . . . . # #
Job 8   |            . . . . . . . # #
Job 3   |  . . . . . . . . . . . . . . # # # #
Job 2   |    . . . . . . . . . . . . . . . . . # # # # #

```
## Preemptive SJF
``` text
Totally 8 jobs, Time using: 23 s
=======================================================================
Gantt Chart
=======================================================================
Time    |0                   1                   2
        |0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9
Job 1   |. #
Job 3   |  . # #
Job 6   |      . #
Job 4   |        . #
Job 5   |      . . . # #
Job 8   |            . . # #
Job 3   |      . . . . . . . # #
Job 7   |            . . . . . . # #
Job 2   |    . . . . . . . . . . . . # # # # #
Job 1   |  . . . . . . . . . . . . . . . . . . # # # # #

```
## RR
``` text
Totally 8 jobs, Time using: 23 s
=======================================================================
Gantt Chart
=======================================================================
Time    |0                   1                   2
        |0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9
Job 1   |. # #
Job 3   |  . . # #
Job 2   |    . . . # #
Job 5   |      . . . . # #
Job 6   |      . . . . . . #
Job 4   |        . . . . . . #
Job 7   |            . . . . . # #
Job 8   |            . . . . . . . # #
Job 1   |                            . # #
Job 3   |                                . # #
Job 2   |                                    . # #
Job 1   |                                        . # #
Job 2   |                                            . #
```
