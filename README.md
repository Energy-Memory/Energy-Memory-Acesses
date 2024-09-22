# Intership-2023-KOPERNIC

Let's see how is organised the repository. 

## Time measures

This folder cointains bash scripts to make or delete the executables of our benchmark. The folder has as well a script to do the execution time measures thanks to the <perf> command.
To proceed, do as follows:
- enable executable modes ```chmod +x <name_of_the_executable>```
- execute bash script to create executables
- put executables in folder named "executables"
- execute bash script to perform time measures

The results are in the folder "time_measures". There is one file per program.

## Results

In this folder, there are time measures and energy consumption files.

## Statistics

There is a c file in this folder that computes statistical results (mean, confidence interval...) of our data. 

To use it, you have to:
- compile it : ```gcc -o stat Statistics.c```
- execute it: ```./stat <current_frequency> <name_of_the_folder_where_time_measures_are_done>```

## Speed policy

This folder has a c file containing an implementation of Quan and Hu's heuristic ("Energy  efficient  fixed-priority  schedulingfor  real-time  systems  on  variable  voltage  processors", 2001).

## Scheduler

This folder is for the scheduler. At first, there is a script to create libraries for the jobs of our real-time systems. Then there are a c file with a job scheduler and makefile.
To proceed, do as follows: 
- execute bash script
- type the following command line : ```export LD_LIBRARY_PATH=/usr/lib:$LD_LIBRARY_PATH```
- compile : ```gcc -L/usr/lib -Wall -o sched Job_Scheduler.c -lndes -lstatemate -lcjpeg_wrbmp``` 

## Energy consumption tests

In this folder there are test programs that run function with few or a lot of memory accesses to check the differences when doing energy consumption.

## UART Communication

The files in this folder are here to test UART communication between the computer and the micro-controller.

## Others

This folder is for other files that we do not use. For now, there is a file named <mouse_handling> which is useful to automate energy consumption measures in the software <Otii 3 App>. However, this automation is according to the size of our monitor.
