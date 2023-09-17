# Process-Scheduler

<div align="center">
  
  [![description](https://img.shields.io/badge/project-Team-1F1F1F?style=for-the-badge)](https://github.com/MUST-Quick-Fry/Process-Scheduler)
  &nbsp;
  [![stars](https://img.shields.io/github/stars/MUST-Quick-Fry/Process-Scheduler?style=for-the-badge&color=FDEE21)](https://github.com/MUST-Quick-Fry/Process-Scheduler/stargazers)
  &nbsp;
  [![forks](https://img.shields.io/github/forks/MUST-Quick-Fry/Process-Scheduler?style=for-the-badge&color=white)](https://github.com/MUST-Quick-Fry/Process-Scheduler/forks)
  &nbsp;
  [![contributors](https://img.shields.io/github/contributors/MUST-Quick-Fry/Process-Scheduler?style=for-the-badge&color=8BC0D0)](https://github.com/MUST-Quick-Fry/Process-Scheduler/graphs/contributors)
  
  <img src="https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black" />
  &nbsp;
  <img src="https://img.shields.io/badge/Ubuntu-E95420?style=for-the-badge&logo=ubuntu&logoColor=white" />
  &nbsp;
  <img src="https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" />
  &nbsp;
  <img src="https://img.shields.io/badge/Visual_Studio-5C2D91?style=for-the-badge&logo=visual%20studio&logoColor=white" />
</div>

<br>

In this project, we are proposed to design a simple process scheduler which be implemented with the function of jobs analysis with different parameters in `.txt` file and involution of process creation, suspension, as well as termination. The programme includes three classes: **Scheduler**, **Monitor** and **job**. We realize 4 scheduling algorithms :
- FIFO
- Non-preemptive SJF
- Preemptive SJF
- RR 



## Table of Contents

- [Development Environment](#development-environment)
- [Install](#install)
- [Usage](#usage)
- [Gantt Chart](#gantt-chart)
- [Maintainers](#maintainers)
- [Contributing](#contributing)
- [License](#license)



## Development Environment

|Attribute|Content|
|-------------------------|-------------------------------|
|*Operation System*|ubuntu-20.04.1-desktop-amd64|
|*Programming Language*| C++11 |
|*Compiler*|g++ version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)|
|*IDE Platform*|Microsoft Visual Studio Community 2019 Version 16.8.3 (Linux Develop)|

> For more specific information regarding the authoring `C++` code for Linux servers, desktops and devices, just click [:cactus:](Process-Scheduler/ReadMe.md)



## Install

```
git clone https://github.com/MUST-Quick-Fry/Process-Scheduler.git
```



## Usage

### Job description file
The standard file format :
>*[arrive time]* `\t` *[Job]* `\t` *[duration time]* `\n`

if you can not open file with invalid error, but it is in standard format, you could use `vim` or `vi` to change the file format as below : 
```code
set ff=unix
```
### Makefile
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
- **Phase 1** (**Monitor**) : change the parameters `target` and `EXEC` in `./Makefile` as 
>```text
> target:=job monitor monitor_main
> EXEC:=Monitor

- **Phase 2** (**Scheduler**) : change the parameters `target` and `EXEC` in `./Makefile` as 
>```text
> target:=utils job monitor scheduler scheduler_main
> EXEC:=Scheduler
### CMD Usage
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



## Gantt Chart
The Gantt Chart followed by job description file `./job_list_example/job4` as example.  
### FIFO
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
### Non-preemptive SJF
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
### Preemptive SJF
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
### RR
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



## Maintainers

![badge](https://img.shields.io/badge/maintenance-NO-EF2D5E) [@goodkillerchen](https://github.com/goodkillerchen), [@KennardWang](https://github.com/KennardWang), [@Tim-eyes](https://github.com/Tim-eyes)



## Contributing

Tasks completed by each person are documented below:

|Name|Task|
|----|----------|
|Chen, Yuxuan (1809853J-I011-0011)|Design the main framework , part of implementation of algorithms|
|Wang, Yuyang (1809853Z-I011-0045)|Implementation of four scheduling algorithms, signal-handler and normalization check of job file|
|He, Peilin (1809853U-I011-0078)|Implementation of monitor time control, pretreat job description file and part of design the framework, report writing|

Sincerely thank our team for the great support, understandings and involvements. 

<table>
  <tr>
    <td align="center"><a href="https://github.com/Tim-eyes"><img src="https://avatars.githubusercontent.com/u/53691717?&v=4" width="100px;" alt=""/><br /><sub><b>@Tim-eyes</b></sub></a><br /><a href="#translation-s8321414" title="Translation"><br />🌍</a></td>
    <td align="center"><a href="https://github.com/KennardWang"><img src="https://avatars.githubusercontent.com/u/57723061?v=4" width="100px;" alt=""/><br /><sub><b>@KennardWang</b></sub></a><br /><a href="#platform-Sandural" title="Packaging/porting to new platform"><br /><img src="https://img.shields.io/badge/Buy_Me_A_Coffee-FFDD00?logo=buy-me-a-coffee&logoColor=black" /></a></td>
    <td align="center"><a href="https://github.com/goodkillerchen"><img src="https://avatars.githubusercontent.com/u/58941606?v=4" width="100px;" alt=""/><br /><sub><b>@goodkillerchen</b></sub></a><br /><a href="https://github.com/CopyTranslator/CopyTranslator/commits?author=Andy-AO" title="Code"><br />💻</a></td>
  </tr>
</table>

Feel free to [open an issue](https://github.com/MUST-Quick-Fry/Process-Scheduler/issues) or submit [PRs](https://github.com/MUST-Quick-Fry/Process-Scheduler/pulls).



## License

[![license](https://img.shields.io/github/license/MUST-Quick-Fry/Process-Scheduler)](LICENSE) © MUST-Quick-Fry ( 2021.8.22 )
