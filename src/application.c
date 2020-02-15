//
// sim.h
// For Assignment 6
// Created by Junzhu Xiang and Haomin Lin on 10/20/2019.
// Copyright © 2019 Junzhu Xiang and Haomin Lin. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sim.h"
#include "initialize.h"
#include <math.h>
#include <string.h>

// Event types
#define	ARRIVAL     1
#define	DEPARTURE   2

/**
 * this function is to get a random number uniformly distributed in [0,1);
 */
double urand() {
    double i = rand() / (double)(RAND_MAX); // 0 < i < 1;
    if (i == 1) {return urand();}
    return i;
}

/**
 * this function is to get a random number from an exponential distribution;
 * P indicates the average arrival or service time;
 */
double randExp(double P) {return -P * (log(1.0 - urand()));}

/**
 * helper function: add a customer to the tail of pump queue;
 */
void addTail(CusInfo *customer, CusInfo *pumpHead) {
    // copy the info of customer to dmp, then free customer;
    CusInfo *dmp = (CusInfo*)malloc(sizeof(CusInfo));
    dmp->QID = customer->QID;
    dmp->in_time = customer->in_time;
    dmp->arrival_time = customer->arrival_time;
    dmp->next = NULL;
    dmp->wait_time = customer->wait_time;
    dmp->EventType = customer->EventType;
    dmp->GID = customer->GID;
    dmp->service_time = customer->service_time;

    // add dmp to the tail of pump list;
    if (pumpHead->next == NULL) {
        pumpHead->next = dmp;
        dmp->next = NULL;
    }else {
        CusInfo *tmp = pumpHead->next;
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
        tmp->next = dmp;
        dmp->next = NULL;
    }
}

/**
 * this function is to remove the first customer in the pump queue;
 */
CusInfo *removePump(CusInfo *pumpHead) {
    CusInfo *tmp;
    if (pumpHead->next == NULL) {return NULL;}
    else {
        tmp = pumpHead->next;
        pumpHead->next = tmp->next;
        return tmp;
    }
}

/**
 * this function is to randomly select a station to schedule the new event according the probability;
 * @return: the index of station in this sub array;
 */
int selectStation(int num, double *prob) {
    //randomly generate a number and select a station;
    double *interval = (double*)malloc((num+1) * sizeof(double));
    for (int i = 0; i < num + 1; i++) {
        interval[i] = 0;
    }
    for (int i = 1; i < num + 1; i++) {
        interval[i] = interval[i - 1] + prob[i - 1];
    }
    double p = (double) rand() /(double) RAND_MAX; // 0 < p < 1
    int tmp; // to record i; indicates the ith item in the FEL;
    for (int i = 0; i < num + 1; i++) {
        if (p >= interval[i] && p < interval[i+1]) {
            tmp = i;
            break;
        }
    }
    free(interval);
    return tmp; // *prob == allProb[i]; so the QID == allConnect[i][tmp];
}

/**
 * this function is to judge this QID is G or not;
 */
int isGenerator(int QID) {
    for (int i = 0; i < G_num; i++) {if (generator[i] == QID) { return 1;}}
    return 0;
}

/**
 * this function is to judge this index is G or not;
 */
int indexIsGenerator(int QID) {
    for (int i = 0; i < G_num; i++) {if (generator_index[i] == QID) { return 1;}}
    return 0;
}

/**
 * a helper function to cal the index of queue station in generator[];
 */
int calGIndex(int QID) {
    if (isGenerator(QID) != 1) {exit(0);}
    for (int i = 0; i < G_num; i++) {if (generator[i] == QID) { return i;}}
}

/**
 * this function is to judge this QID is E or not;
 */
int isExit(int QID) {
    for (int i = 0; i < E_num; i++) {if (Exit[i] == QID) { return 1;}}
    return 0;
}

/**
 * this function is to deal with the arrival event;
 * for enter arrival, it's just like the arrival in sample code;
 */
void Arrival(CusInfo *customer) {
    printf("Processing Arrival Event at time: At %lf, Station: %d, %d customers waiting\n",
           CurrentTime(), customer->QID, numPump[customer->QID]);

    double ts; // timestamp;
    //check if the info is correct or not;
    if (customer->EventType != ARRIVAL) {
        printf("Unexpected event type!\n");
        exit(0);
    }
    // schedule next arrival event;
    if (isGenerator(customer->QID) == 1) { // means this is a generator;
        CusInfo *d = (CusInfo *) malloc(sizeof(CusInfo));
        if (d == NULL) {
            printf("malloc failed!\n");
            exit(0);
        }
        int index = calGIndex(customer->QID);
        ts = CurrentTime() + randExp(generator_AT[index]); // randExp(ave_arrival_time);
        // d is the new event;
        d->EventType = ARRIVAL;
        d->QID = customer->QID;
        d->next = NULL;
        d->wait_time = 0; // when first arrival, wait time is 0;
        d->arrival_time = ts; // change when customer goes to next station;
        d->in_time = ts; // does not change over time;
        d->service_time = 0;
        d->GID = customer->GID;
        Schedule(ts, d);
        in_num++; // indicates a new customer enters the system;
    }
    // if the Pump is free, customer will use the pump for a specific time unit; schedule departure event
    if (numPump[customer->QID] == 0) {
        CusInfo *d = (CusInfo *) malloc(sizeof(CusInfo));
        double st = randExp(ST[customer->QID]);
        ts = CurrentTime() + st;
        d->EventType = DEPARTURE;
        d->GID = customer->GID;
        d->QID = customer->QID;
        d->wait_time = customer->wait_time;
        d->arrival_time = customer->arrival_time;
        d->in_time = customer->in_time;
        d->next = NULL;
        d->service_time = st;
        Schedule(ts, d);
    }
    // if the current station is full, add the customer to the pumplists;
    else if (numPump[customer->QID] > 0) {addTail(customer, pumpList[customer->QID]);}
    numPump[customer->QID]++;
}

/**
 * this function is to deal with the departure event of the enter station;
 */
void Departure(CusInfo *customer) {
    printf("Processing Departure Event at time: %lf, Station: %d, %d customers waiting\n",
           CurrentTime(), customer->QID, numPump[customer->QID]);
    double ts;
    //check if the info is correct or not;
    if (customer->EventType != DEPARTURE) {
        printf("Unexpected event type!\n");
        exit(0);
    }
    numPump[customer->QID]--;

    departure_list[customer->QID]++;
    each_wait_time[customer->QID] += CurrentTime() - customer->arrival_time - customer->service_time;

    //schedule the departure of the customer that is firstly waiting to use this enter station;
    if (numPump[customer->QID] > 0) {
        CusInfo *tmp = removePump(pumpList[customer->QID]); if (tmp == NULL) {exit(0);}
        CusInfo *e = (CusInfo *) malloc(sizeof(CusInfo));
        double st = randExp(ST[customer->QID]);
        ts = CurrentTime() + st;
        e->EventType = DEPARTURE;
        e->GID = tmp->GID;
        e->QID = tmp->QID; if (tmp->QID != customer->QID) {exit(0);}
        e->next = NULL;
        e->service_time = st;
        e->wait_time = CurrentTime() - tmp->arrival_time + tmp->wait_time;
        e->arrival_time = tmp->arrival_time; // does not matter; because when inject, we have to re-assign it;
        e->in_time = tmp->in_time;
        Schedule(ts, e);
        free(tmp); //free the memory occupied by the first customer in the queue
    }
}

/**
 * this function is to inject customer to exit;
 */
void InjectToExit(CusInfo *customer) {
    if (isExit(customer->QID) == 0 || customer->EventType != DEPARTURE) {exit(1111);}
    out_num++;
    // cal the in-time for customers;
    total_in_time += CurrentTime() - customer->in_time;
    if (min_in_time == 0) {min_in_time = CurrentTime() - customer->in_time;}
    else {
        if (min_in_time >= CurrentTime() - customer->in_time) {min_in_time = CurrentTime() - customer->in_time;}
    }
    if (max_in_time <= CurrentTime() - customer->in_time) {max_in_time = CurrentTime() - customer->in_time;}
    // cal the wait time for customers;
    total_wait_time += customer->wait_time;
    if (min_wait_time == -1) {min_wait_time = customer->wait_time;}
    else if (min_wait_time >= customer->wait_time) {min_wait_time = customer->wait_time;}
    if (max_wait_time <= customer->wait_time) {max_wait_time = customer->wait_time;}
}

/**
 * this function is to inject event data to next station after Departure(e);
 */
void Inject(CusInfo *customer) {
    if (customer->EventType == DEPARTURE) { // not in the check out station;
        int station_num;
        int index;
        //compute the number of station where customer should be;
        index = selectStation(connect_num[customer->QID], allProb[customer->QID]);
        station_num = allConnect[customer->QID][index];
        if (isExit(station_num) == 1) {
            customer->QID = station_num;
            customer->EventType = DEPARTURE;
            InjectToExit(customer);
        }
        else {
            CusInfo *d = (CusInfo*)malloc(sizeof(CusInfo));
            // copy value, then inject;
            double ts = CurrentTime();
            d->EventType = ARRIVAL;
            d->QID = station_num;
            if (station_num == -1) {exit(0);}
            d->GID = customer->GID;
            d->next = NULL;
            d->in_time = customer->in_time;
            d->arrival_time = ts;
            d->wait_time = customer->wait_time;
            d->service_time = 0;
            Schedule(ts, d);
        }
    }
}

/**
 * This function is called by the simulation engine to process an event removed from the future event list;
 */
void eventHandler(void *AppData) {
    CusInfo *d;
    d = (CusInfo*)AppData;
    if (d->EventType == ARRIVAL) {
        Arrival(d);
        free(d); //free the memory used temporarily
    }
    else if (d->EventType == DEPARTURE) {
        Departure(d);
        Inject(d);
        free(d); //free the memory used temporarily
    }
    else {printf("Illegal event found\n"); exit(0);}
}

/**
 * this is a function to compute the min, max, ave waiting time after the RunSim function;
 * In injectToExit function, we have computed the wait time of customers who have exited;
 * go through all the pump linked lists to get the wait time of customers who still in the system;
 */
void calWaitTime() {
    for (int i = 0; i < Q_num; i++) {
        if (pumpList[i]->next == NULL) {continue;}
        CusInfo *tmp = pumpList[i]->next;
        while (tmp != NULL) {
            double wt = tmp->wait_time + (EndTime - tmp->arrival_time);
            total_wait_time = total_wait_time + wt;
            if (min_wait_time >= wt) {min_wait_time = wt;}
            if (max_wait_time <= wt) {max_wait_time = wt;}
            tmp = tmp->next;
        }
    }
}

/**
 * first read data from the configuration file;
 * after the first time read, we can get the G_num, Q_num, E_num;
 */
void firstReadData(char *file_name) {
    FILE *fread = fopen(file_name, "r");
    Q_num = 0; G_num = 0; E_num = 0;
    fscanf(fread, "%d", &C_num);
    //initial the array storing the number of each station's connections
    initConnectNum();
    //initial the array storing all the ID numbers of stations
    initallID();
    
    int ID;
    char *type1 = (char*)malloc(1 + sizeof(char)); // record the component's type;
    char *G = "G"; char *Q = "Q"; char *E = "E";
    double time; int t; double tmp;
    //read C_num lines from now on;
    for (int i = 0; i < C_num; i++) {
        fscanf(fread, "%d", &ID);
        fscanf(fread, "%s", type1);
        //check if the ID of a station exceeds the max ID number allowed in the system
        if(ID > C_num - 1){
            printf("Error 10: ID at line #%d exceeds the limit\n", i);
            exit(10);
        }
        // get the information of each component;
        if (strcmp(type1, G) == 0) {
            G_num++;
            Q_num++;
            fscanf(fread, "%lf", &time);
            fscanf(fread, "%d", &t);
            t = 0;
            connect_num[ID] = t;
        }
        else if (strcmp(type1, Q) == 0) {
            Q_num++;
            fscanf(fread, "%lf", &time);
            fscanf(fread, "%d", &t);
            //define the number of stations
            connect_num[ID] = t;
        }
        else if (strcmp(type1, E) == 0) {
            E_num++;
            connect_num[ID] = 0;
        }
        for (int j = 0; j < 2 * t; j++) {fscanf(fread, "%lf", &tmp);}
    }
    //free space
    free(type1);
    fclose(fread);
}

/**
 * in the second time, we shall init some arrays;
 */
void secondReadData(char *file_name) {
    FILE *fread = fopen(file_name, "r");
    fscanf(fread, "%d", &C_num);
    int ID = 0;
    char *type = (char*)malloc(1 + sizeof(char)); // record the component's type;
    char *G = "G"; char *Q = "Q"; char *E = "E";
    int t;
    int g = -1; // indicate the num of G;
    int e = -1; // indicate the num of E;
    int q = -1; // indicate the num of Q;
    //read C_num lines from now on;
    for (int i = 0; i < C_num; i++) {
        fscanf(fread, "%d %s", &ID, type);
        all_id[i] = ID;
        // get the info about each G
        if (strcmp(type, G) == 0) {
            g++;
            fscanf(fread, "%lf", &generator_AT[g]);
            generator_index[g] = ID;
            ST[ID] = 0;
            fscanf(fread, "%d", &generator[g]);
            connect_num[ID] = 1;
        }
        // get the info about each Q;
        else if (strcmp(type, Q) == 0) {
            q++;
            queue_index[q] = ID;
            fscanf(fread, "%lf", &ST[ID]);
            fscanf(fread, "%d", &t);
            connect_num[ID] = t;
            for (int j = 0; j < t; j++) {fscanf(fread, "%lf", &allProb[ID][j]);}
            for (int k = 0; k < t; k++) {fscanf(fread, "%d", &allConnect[ID][k]);}
        }
        else if (strcmp(type, E) == 0) {
            e++;
            Exit[e] = ID;
        }
    }
    //free space
    free(type);
    fclose(fread);
}

/**
 * This function free all the allocated memory that needs to be freed to avoid memory leak
 */
void Free_all(void){
    //Free the space of the array storing all the station's connected stations
    for (int i = 0; i < C_num; i++) {
        free(allConnect[i]);
        allConnect[i] = NULL;
    }
    free(allConnect);
    allConnect = NULL;
    //Free the space of the array storing all the station's connected stations' probablities
    for (int j = 0; j < C_num; j++) {
        free(allProb[j]);
        allProb[j] = NULL;
    }
    free(allProb);
    allProb = NULL;
    //Free the space of the array storing all the ID numbers
    free(all_id);
    all_id = NULL;
    //Free the space of the array storing the number of each station's connected stations
    free(connect_num);
    connect_num = NULL;
    //Free the space of the array storing the ID of the queuing stations
    free(queue_index);
    queue_index = NULL;
    //Free the space of the array storing all the stations' service time
    free(ST);
    ST = NULL;
    //Free the space of the array storing the Exit's ID number
    free(Exit);
    Exit = NULL;
    //Free the space of the array storing the generators' connecting stations' number
    free(generator);
    generator = NULL;
    //Free the space of the array storing the generators‘ ID number
    free(generator_index);
    generator_index = NULL;
    //Free the space of the processing time of generators
    free(generator_AT);
    generator_AT = NULL;
    //Free the space of the array storing the customers that leave the system
    free(departure_list);
    departure_list = NULL;
    //Free the space of the waiting time of each customers
    free(each_wait_time);
    each_wait_time = NULL;
    //Free the space of the array storing number of customers waiting at each station
    free(numPump);
    numPump = NULL;
    //Free the space containing the customers that are still waiting
    for (int k = 0; k < C_num; k++){
        CusInfo* p; //declare a pointer to save the temporary address of node to be deleted
        //Keep deleting until the array is empty
        while(pumpList[k]->next != NULL){
            p = pumpList[k]->next;
            pumpList[k]->next = p->next;
            free(p);
        }
        free(pumpList[k]); //free the head
        pumpList[k] = NULL;
    }
    free(pumpList);
    pumpList = NULL;
    printf("\nFree is done\n");
}

/**
 * This function checks if the input configuration file is legal,
 * if not, the program will report error and exit
 */
void check_errors(){
    //Check if the total number is missing
    if(C_num == 0){
        printf("Error 10: Missing total number!\n");
        exit(10);
    }
    //Check if the numbers of generators, queuing stations and exit sum up to be the same as total number
    if(C_num != Q_num + E_num){
        printf("Error 1: there should be %d components, but only %d Generators, %d Queues, %d Exits, which sums up to %d\n", C_num, G_num, Q_num-G_num, E_num, Q_num+E_num);
        exit(1);
    }
    //check if there exists an Exit
    if(E_num == 0){
        printf("Error 5: Missing Exit!\n");
        exit(5);
    }
    //check if there exists any queuing stations
    if(Q_num == 0){
        printf("Error 5: Missing Queues!\n");
        exit(5);
    }
    //check if there exists any generators
    if(G_num == 0){
        printf("Error 5: Missing Generators!\n");
        exit(5);
    }
    //check if all the information for the components are filled
    for (int n = 0; n < Q_num - G_num; n++){
        for(int m = 0; m < connect_num[queue_index[n]]; m++){
            if(allConnect[queue_index[n]][m] == -1){
                printf("queue%d\n", queue_index[n]);
                printf("Error 3: Incomplete information for components\n");
                exit(3);
            }
        }
    }
    //check if the ID number of each station is in order as the assignment indicates
    for(int i = 0; i < C_num; i++){
        if(all_id[i] != i){
            printf("Error 8: Station %d has number out of order\n", i);
            exit(8);
        }
    }
    //check if a generator connects to itself
    for(int j = 0; j < G_num; j++){
        if (generator[j] == generator_index[j]) {
            printf("Error 7: #%d Generator connects to itself!\n", generator[j]);
            exit(7);
        }
    }
    //check if the ID number of each station is unique
    for(int i = 0; i < G_num; i++){
        int test_co = generator_index[i];
        for(int l = 0; l < C_num; l++){
            if(l == generator_index[i]){
                continue;
            }
            if(all_id[l] == test_co){
                printf("Error 6: #%d Generator's number has duplicate!\n", test_co);
                exit(6);
            }
        }
    }
    for(int i = 0; i < Q_num - G_num; i++){
        int test_co = queue_index[i];
        for(int l = 0; l < C_num; l++){
            if(l == queue_index[i]){
                continue;
            }
            if(all_id[l] == test_co){
                printf("Error 6: #%d Queue's number has duplicate!\n", test_co);
                exit(6);
            }
        }
        //check if a queue connects to itself
        for (int n = 0; n < connect_num[i]; n++){
            if(allConnect[test_co][n] == test_co){
                printf("Error 7: #%d Queue connects to itself!\n", test_co);
                exit(7);
            }
        }
    }
    for(int i = 0; i < E_num; i++){
        int test_co = Exit[i];
        for(int l = 0; l < C_num; l++){
            if(l == Exit[i]){
                continue;
            }
            if(all_id[l] == test_co){
                printf("Error 6: #%d Exit's number has duplicate!\n", test_co);
                exit(6);
            }
        }
    }
    //check if the probabilities of each stations' connecting stations are between 0 and i and sum up to 1
    for (int j = 0; j < Q_num - G_num; j++){
        double t_prob = 0;
        for(int i = 0; i < connect_num[queue_index[j]]; i++){
            if(allProb[queue_index[j]][i] > 1 || allProb[queue_index[j]][i] <= 0){
                printf("Error 4: #%d Station's #%d connection probability is illegal, it should be between (0, 1], but it's now %lf \n", j, i, allProb[queue_index[j]][i]);
                exit(4);
            }
            t_prob = t_prob + allProb[queue_index[j]][i];
        }
        if(t_prob != 1){
            printf("Error 2: #%d Station's total connecting probability is wrong, it should sum up to 1\n", j);
            exit(2);
        }
    }
}

/**
 * this function is to write data to the output file;
 */
void writeData(char *outfile) {
    char file_path[100];
    FILE **cluster_files = (FILE**)malloc(sizeof(FILE*)); //points to each cluster file which stores cluster data;
    if (cluster_files == NULL) {
        printf("Creating cluster_files failed!\n");
        exit(0);
    }
    //write file path to file_path[]
    sprintf(file_path, outfile);
    cluster_files[0] = fopen(file_path, "w");
    //write attribute
    fprintf(cluster_files[0], "the number of customers entering the system:\n%d\n", in_num);
    fprintf(cluster_files[0], "the number of customers exiting the system:\n%d\n", out_num);
    fprintf(cluster_files[0], "\nThe minimum/maximum/average time of each customer staying the system:\n%lf %lf %lf\n",
            min_in_time, max_in_time,total_in_time/out_num);
    fprintf(cluster_files[0], "The minimum/maximum/average time of one customer waiting in queues:\n%lf %lf %lf\n",
            min_wait_time, max_wait_time,total_wait_time/in_num);

    int flag_num = -1;
    int flag = 0;
    fprintf(cluster_files[0],"\nThe average waiting time experienced by customers at each station:\n");
    for (int i = 0; i < Q_num; i++) {
        if (indexIsGenerator(i) == 1) { continue;}
        flag_num++;
        flag = queue_index[flag_num];
        fprintf(cluster_files[0], "Queueing Station ID: %d, Time: %lf \n", flag, each_wait_time[flag]/departure_list[flag]);
    }
    //after writing, close ths file;
    fclose(cluster_files[0]);
    free(cluster_files);
}

int main(int argc, char ** argv) {
    printf("This program is: %s\n",argv[0]);
    if (argc < 4) {
        printf("You need to input three parameters: EndTime, input_file, and output_file!\n");
        exit(0);
    }
    // get the values from command line;
    EndTime = atof(argv[1]); // EndTime should be a positive value;
    if (EndTime <= 0) {printf("Error! EndTime should be larger than 0!\n"); exit(0);}
    char *input_file_name = argv[2];
    char *output_file_name = argv[3];

    // code below is to get basic values and then init arrays that will be used later;
    firstReadData(input_file_name);
    srand(100000000 * (unsigned int)time(0));
    initFEL();
    initPump();
    initGeneratorQueue();
    initExit();
    initST();
    initConnectProb();
    secondReadData(input_file_name);

    //run check_errors to check if the configuration file is legal
    check_errors();
    initTotalWaitTime();

    // code below is to add first G_num customers to the system to start the system;
    double ts;
    double start_time = 10;
    for (int i = 0; i < G_num; i++) {
        CusInfo *d =(CusInfo*)malloc (sizeof(CusInfo));
        // initialize event list with first arrival
        d->EventType = ARRIVAL;
        d->GID = i;
        d->QID = generator[i];
        //the arrival time is 10 here, you can set it as what you like
        d->in_time = start_time;
        d->wait_time = 0;
        d->service_time = 0;
        d->arrival_time = start_time;
        d->next = NULL;
        ts = start_time;
        Schedule (ts, d);
    }
    if (EndTime <= start_time) {
        printf("EndTime should be larger than StartTime!\n");
        exit(0);
    }
    // start to run the simulation;
    RunSim(EndTime);
    calWaitTime(); // compute the wait time among those still in the system;

    printf("\nNumber of queueing stations: %d\n", Q_num-G_num);
    printf("Number of generators: %d\n", G_num);
    printf("Number of exits: %d\n", E_num);

    printf("\nThe number of customers entering the system: %d\n",in_num);
    printf("The number of customers exiting the system: %d\n",out_num);

    printf("\nThe minimum/maximum/average time of one customer in the system:\n%lf\n%lf\n%lf\n", min_in_time, max_in_time,total_in_time/out_num);
    printf("The minimum/maximum/average time of one customer waiting in queues:\n%lf\n%lf\n%lf\n", min_wait_time, max_wait_time,total_wait_time/in_num);

    printf("\nThe average waiting time experienced by customers at each station:\n");

    int flag_num = -1;
    int flag = 0;
    for (int i = 0; i < Q_num; i++) {
        if (indexIsGenerator(i) == 1) { continue;}
        flag_num++;
        flag = queue_index[flag_num];
        printf("Queueing Station ID: %d, Time: %lf \n", flag, each_wait_time[flag]/departure_list[flag]);
    }

    // write data to output file;
    writeData(output_file_name);

    // free the future event lists;
    while(allFEL->next != NULL)
    {
        Event* p = allFEL->next; /* assign the pointer to the node to be deleted */
        allFEL->next = p->next; /* connect the head with nodes after the one to be deleted */
        free(p->AppData);
        p->AppData = NULL;
        free(p); /* free the node to be deleted */
        p = NULL;
    }
    free(allFEL->AppData);
    allFEL->AppData = NULL;
    free(allFEL); /* free the head */
    allFEL = NULL;

    Free_all();

    return 0;
}


