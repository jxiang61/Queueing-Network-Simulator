//
// engine.c
// For Assignment 6
// Created by Junzhu Xiang and Haomin Lin on 10/20/2019.
// Copyright © 2019 Junzhu Xiang and Haomin Lin. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "sim.h"
#include "initialize.h"

int size = 0; //the size of each FEL;

/**
 * this function is to record current time;
 * @source: Sample Code from CSE 6010;
 */
double CurrentTime () {return (now);}

/**
 * this function is to remove the smallest timestamp Event in FEL, which is the front one;
 * @return: the pointer that points to the event that has been removed;
 */
Event *dequeue() {
    if (allFEL->next == NULL) {
        return NULL;
    } else {
        Event *tmp;
        tmp = allFEL->next;
        allFEL->next = tmp->next;
        return tmp;
    }
}

/**
 * Print timestamps of all events in the event list (used for debugging)
 * @source: Sample Code from CSE 6010;
 */
void PrintList (Event *FELHead) {
    struct Event *p;
    CusInfo *d;
    printf ("Event List: ");
    for (p=FELHead->next; p != NULL; p=p->next) {
        d = p->AppData;
        if (d->EventType == 1) {printf ("%f(EventType: ARRIVAL, QID:%d) ", p->timestamp, d->QID);}
        else if (d->EventType == 2) {printf ("%f(EventType: DEPARTURE, QID:%d) ", p->timestamp, d->QID);}
    }
    printf ("\n");
}

/**
 * this function is to schedule a new event in FEL (future event that has not been happen);
 * The FEL queue is a priority queue; timestamp is the priority;
 */
void Schedule(double timestamp, void *EventData) {
    // create and allocate a new event by using these parameters above;
    Event *tmp = (Event*)malloc(sizeof(Event));
    if (tmp == NULL) {
        printf("schedule failed!\n");
        exit(0);
    }
    // passing parameters to this tmp Event;
    tmp->AppData = EventData;
    tmp->timestamp = timestamp;
    // add tmp to this FEL according to its timestamp priority;
    Event *h, *t;
    for (h = allFEL, t = allFEL->next; t != NULL; t = t->next, h = h->next) {
        if (t->timestamp >= tmp->timestamp){
            break;
        }
    }
    if (t == NULL || t->timestamp > tmp->timestamp) {
        tmp->next = h->next;
        h->next = tmp;
    }
    else if (t->timestamp == tmp->timestamp) {
        tmp->next = t->next;
        t->next = tmp;
    }
}

/**
 * this is a function to execute simulation up to a specified time (EndTime)
 */
void RunSim (double EndTime) {
    Event *e;
    printf("Initial event list:\n");
    PrintList (allFEL);

    while ((e = dequeue()) != NULL) {
        now = e->timestamp;
        if (now >= EndTime) {
            free(e->AppData);
            free(e);
            break;
        }
        eventHandler(e->AppData);
        free(e);
        e = NULL;
        printf("\n");
        PrintList(allFEL);
    }
}


