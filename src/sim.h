//
// sim.h
// For Assignment 6
// Created by Junzhu Xiang and Haomin Lin on 10/20/2019.
// Copyright © 2019 Junzhu Xiang and Haomin Lin. All rights reserved.
//

#ifndef A6_V1_SIM_H
#define A6_V1_SIM_H

/**
 * define the structure of event in the FEL;
 */
typedef struct Event {
    double timestamp;
    void *AppData; // points to the specific application data;
    struct Event *next; // points to the next Event in the FEL;
} Event;

/**
 * This function is to record current time;
 * @source: Sample Code from CSE 6010;
 */
double CurrentTime ();

/**
 * This function is to remove the smallest timestamp Event in FEL, which is the front one;
 * @return: the pointer that points to the event that has been removed;
 */
Event *dequeue();

/**
 * Print timestamps of all events in the event list (used for debugging)
 * @source: Sample Code from CSE 6010;
 */
void PrintList (Event *FELHead);

/**
 * This function is to schedule a new event in FEL (future event that has not been happen);
 * The FEL queue is a priority queue; timestamp is the priority;
 */
void Schedule(double timestamp, void *EventData);

/**
* This function is called by the simulation engine to process an event removed from the future event list;
*/
void eventHandler(void *AppData);

/**
 * This is a function to execute simulation up to a specified time (EndTime)
 */
void RunSim (double EndTime);

/**
 * This is a function to free all the allocated memory space that still needs to be free
 * at the end of the program
 * It can avoid memory leak
 */
void Free_all();

/**
 * This is a function to check the configuration file is sufficiently legal
 * if the configuratino file is illegal, the program will report the error and exit
 */
void check_errors();

#endif //A6_V1_SIM_H
