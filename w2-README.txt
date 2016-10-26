Documentation for Warmup Assignment 2
=====================================

+-------+
| BUILD |
+-------+

Comments: Use "make warmup2" or just "make" command to create the executable to run the program.

+-------+
| SKIP  |
+-------+

N/A

+---------+
| GRADING |
+---------+

Basic running of the code : 100 out of 100 pts

Missing required section(s) in README file : Nil
Cannot compile : Nil
Compiler warnings :Nil
"make clean" : Nil
Segmentation faults : Nil
Separate compilation : Nil
Using busy-wait : Nil
Handling of commandline arguments:
    1) -n : Nil
    2) -lambda : Nil
    3) -mu : Nil
    4) -r : Nil
    5) -B : Nil
    6) -P : Nil
Trace output :
    1) regular packets: 7 lines
    2) dropped packets: 1 line
    3) removed packets: Printning removed packets
    4) token arrival (dropped or not dropped): 
Statistics output :
    1) inter-arrival time : Matches the Trace Value
    2) service time : Matches the Trace Value
    3) number of customers in Q1 : Matches the Trace Value
    4) number of customers in Q2 : Matches the Trace Value
    5) number of customers at a server : Matches the Trace Value
    6) time in system : Matches the Trace Value
    7) standard deviation for time in system : Matches the Trace Value
    8) drop probability : Matches the Trace Value
Output bad format : Nil
Output wrong precision for statistics (should be 6-8 significant digits) : Nil
Large service time test : Works fine
Large inter-arrival time test : Works fine
Tiny inter-arrival time test : small deviation from actual value (Due to thread wakeup)
Tiny service time test : small deviation from actual value (Due to thread wakeup)
Large total number of customers test : Works fine
Large total number of customers with high arrival rate test : Works fine
Dropped tokens test : Works fine
Cannot handle <Cntrl+C> at all (ignored or no statistics) : Nil
Can handle <Cntrl+C> but statistics way off : Nil
Not using condition variables and do some kind of busy-wait : Nil
Synchronization check : Nil
Deadlocks : Nil

+------+
| BUGS |
+------+

Nil

+--------+
| OTHER  |
+--------+
Files
Makefile: To generate the extecutable
main.c: Thread initialization, printing statistics and handles the command line arguments and read from file
global.h/defs.h/cs402.h: contains all the global variables and include directories
my402list.c/my402list.h: Double linked list implementation
packet.c:Takes care of all packet handling functions 
server1.c:Takes care of servicing of packets
token.c:Takes care of taken arrival
signalhandler.c: Takes care of <cntrl+c> case

Comments on design decisions: Cntrl+C handled using sigwait. When Cntrl+C is pressed it will update global variables so that other threads can quit.Packet thread quits after all the packets generated are.
Comments on deviation from spec: Nil


