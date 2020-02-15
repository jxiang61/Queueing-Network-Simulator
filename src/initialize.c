//
// initialize.c
// For Assignment 6
// Created by Junzhu Xiang and Haomin Lin on 10/20/2019.
// Copyright © 2019 Junzhu Xiang and Haomin Lin. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "sim.h"
#include "initialize.h"

// event types
#define	ARRIVAL     1
#define	DEPARTURE   2

int in_num = 0; // indicate the number of customers entering the system;
int out_num = 0;// indicate the number of customers exiting the system;

double now = 0; // the current time;

int Q_num = 0; // indicate the num of all stations in this system; not for the G or E;
int G_num = 0; // the number of generators; Q_num should include G_num;
int E_num = 0; // the number of Exits;
int C_num = 0; // the number of all components in this system;

double total_in_time = 0; // the total time of customers staying in the system;
double min_in_time = 0; // the min time of customers staying in the system;
double max_in_time = 0; // the max time of customers staying in the system;

double total_wait_time = 0; //the total time of customers waiting in queue stations;
double min_wait_time = -1; // the initial min time of customers waiting in queue stations;
double max_wait_time = 0; // the max time of customers waiting in queue stations;

/**
 * Function to init all_id;
 */
void initallID(){
    all_id = (int*)malloc(C_num * sizeof(int));
}

/**
 * Function to init each_wait_time and departure_list;
 */
void initTotalWaitTime() {
    each_wait_time = (double*)malloc(C_num * sizeof(double));
    departure_list = (int*)malloc(C_num * sizeof(int));
    for (int i = 0; i < C_num; i++) {
        each_wait_time[i] = 0;
        departure_list[i] = 0;
    }
}

/**
 * this function is to init a two dimension array to store all enter FEL;
 */
void initFEL() {
    //allocate; the head node of FEL;
    allFEL = (Event*)malloc(sizeof(Event));
    if (allFEL == NULL) {
        printf("init allEnterFEL failed!\n");
        exit(0);
    }
    allFEL->timestamp = -1;
    allFEL->AppData = NULL;
    allFEL->next = NULL;
}

/**
 * this function is to init arrays to store the pump information;
 */
void initPump() {
    numPump = (int*)malloc(C_num * sizeof(int));
    pumpList = (CusInfo**)malloc(C_num * sizeof(CusInfo*));
    for (int i = 0; i < C_num; i++) {
        numPump[i] = 0;
        pumpList[i] = (CusInfo*)malloc(sizeof(CusInfo));
        pumpList[i]->QID = i;
        pumpList[i]->next = NULL;
        pumpList[i]->GID = -1;
        pumpList[i]->EventType = ARRIVAL;
        pumpList[i]->in_time = -1;
        pumpList[i]->wait_time = -1;
        pumpList[i]->arrival_time = -1;
        pumpList[i]->service_time = -1;
    }
}

/**
 * this function is to init arrays to store the index of generators and queues;
 */
void initGeneratorQueue() {
    generator = (int*)malloc(G_num * sizeof(int));
    generator_AT = (double*)malloc(G_num * sizeof(double));
    generator_index = (int*)malloc(G_num * sizeof(int));
    queue_index = (int*)malloc((Q_num-G_num) * sizeof(int));
    for (int i = 0; i < G_num; i++) {generator[i] = 0; generator_AT[i] = 0; generator_index[i] = 0;}
    for (int i = 0; i < Q_num - G_num; i++) {queue_index[i] = 0;}
}

/**
 * this function is to init arrays to store the index of generators;
 */
void initExit() {
    Exit = (int*)malloc(E_num * sizeof(int));
    for (int i = 0; i < E_num; i++) {Exit[i] = 0;}
}

/**
 * this function is to init array to store the service time for each station;
 */
void initST() {
    ST = (double*)malloc(C_num * sizeof(double));
    for (int i = 0; i < C_num; i++) {ST[i] = 0;}
}

/**
 * this function is to init an array to store the number of stations each station connected to;
 */
void initConnectNum() {
    connect_num = (int*)malloc(C_num * sizeof(int));
    for (int i = 0; i < C_num; i++) {connect_num[i] = 0;}
}

/**
 * this function is to init allConnect and allProb;
 */
void initConnectProb() {
    allConnect = (int**)malloc(C_num * sizeof(int*));
    for (int i = 0; i < C_num; i++) {
        allConnect[i] = (int*)malloc(connect_num[i] * sizeof(int));
        for(int j = 0; j < connect_num[i]; j++){
            allConnect[i][j] = -1;
        }
    }
    allProb = (double**)malloc(C_num * sizeof(double*));
    for (int i = 0; i < C_num; i++) {
        allProb[i] = (double *)malloc(connect_num[i] * sizeof(double));
        for(int j = 0; j < connect_num[i]; j++){
            allProb[i][j] = -1;
        }
    }
}

