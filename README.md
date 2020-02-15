# Queueing-Network-Simulator
This is a discrete event simulation (DES) program to simulate the queueing network.

Authors: Junzhu Xiang, Haomin Lin.
Created: 10/20/2019
Last modified: 11/11/2019
Affiliation: Georgia Institute of Technology
Operating system: Linux

Description:
-------------
In this assignment, we created a discrete time event system. To run this prorgam, first you should
input a file containing information about the components of this system, then you can get an output
file about customers' information after running this program.

Installation:
------------
(1) Run:
------------
gcc -lm application.c engine.c initialize.c -std=c99 -o A6

Note: The -std=c99 flag is required on some C compilers to allow variable declarations at arbitrary locations in a function.

(2) Execution:
------------
./A6 240 input_file.txt output_file.txt

Note: 240 indicate the EndTime of this simulation, you can change it with a double type number you want, but it cannot be a negative number or 0. 
And the EndTime must be larger than start time, in this program, the start time is 10. input file is the configuration file that stores the 
information about components of this system. output file is the file we can get after running this program.
