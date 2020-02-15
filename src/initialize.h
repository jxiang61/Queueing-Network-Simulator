//
// initialize.h
// For Assignment 6
// Created by Junzhu Xiang and Haomin Lin on 10/20/2019.
// Copyright © 2019 Junzhu Xiang and Haomin Lin. All rights reserved.
//

#ifndef A6_V3_INITIALIZE_H
#define A6_V3_INITIALIZE_H

/**
 * define the structure that stores the information of each event, here customer;
 */
typedef struct EventData {
    int EventType; // arrival or departure;
    double arrival_time; // a helper invariant to compute the wait_time;
    double wait_time; // the total waiting time for a customer so far;
    int QID; // to indicate which Q station the customer belongs;
    int GID; // to indicate which G station the customer belongs to;
    double in_time; // to record the time when the customer first enters the system; does not change over time;
    double service_time; // to record the service time the customer will receive in the current station;
    struct EventData *next;
} CusInfo;

extern double now; // the current time;

Event *allFEL; // an array to store all future events;

int *numPump; // an array to store the number of customers at each station;
CusInfo **pumpList; // an array to store customers waiting in the pump;

extern int Q_num; // indicate the num of all stations in this system; not for the G or E;
extern int G_num; // the number of generators; Q_num should include G_num;
extern int E_num; // the number of Exits;
extern int C_num; // the number of all components in this system;

extern int in_num; // indicate the number of customers entering the system;
extern int out_num;// indicate the number of customers exiting the system;

extern double total_in_time;
extern double min_in_time;
extern double max_in_time;

extern double total_wait_time;
extern double min_wait_time;
extern double max_wait_time;

double EndTime;

double *each_wait_time; // to record the total waiting time for each station among all customers;
int *departure_list; // to record the number of departure events which have happened in each station;

int *generator;// array to store the index of Q station connecting to generator;
double *generator_AT; // to store the arrival time of each generator;
int *generator_index; // to store the index of each generator;
int *Exit;// array to store the index of exit;
double *ST;// an array to store the service time for each station;
int *connect_num;// an array to store the number of each station connected to;
int **allConnect;// 2D array to store the info for all stations' connection;
double **allProb;// 2D array to store the prob info for all stations' connection;
int *queue_index; // to store the index of each queue station;

int *all_id; // to store all the ID numbers of stations;

/**
 * Function to init all_id;
 */
void initallID();

/**
 * this function is to init arrays to store the index of generators and queues;
 */
void initGeneratorQueue();

/**
 * this function is to init a two dimension array to store all enter FEL;
 */
void initFEL();

/**
 * this function is to init arrays to store the pump information;
 */
void initPump();

/**
* this function is to init arrays to store the index of generators;
*/
void initExit();

/**
 * this function is to init array to store the service time for each station;
 */
void initST();

/**
 * this function is to init an array to store the number of stations each station connected to;
 */
void initConnectNum();

/**
 * this function is to init allConnect and allProb;
 */
void initConnectProb();

/**
 * Function to init each_wait_time and departure_list;
 */
void initTotalWaitTime();

#endif //A6_V3_INITIALIZE_H
