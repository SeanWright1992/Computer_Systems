//Sean Wright
//cpsc 3500
//sections1.c: mutual exclusion model sections

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "sections.h"
#include "mdat.h"


// TODO: Declare shared variables here

pthread_mutex_t lock;
int* forks; 

void sectionInitGlobals(int numPhilosophers)
{
	const char* TRACE = "Lock trace";
	int i;
	mdat_mutex_init(TRACE, &lock, NULL);
	forks = (int*) malloc(numPhilosophers* sizeof(int));
	for(i = 0; i < numPhilosophers; i++)
		forks[i] = 0;
}

void sectionRunPhilosopher(int philosopherID, int numRounds)
{
	int i;
	int numSticks = 0;
	for (i = 0; i < numRounds; i++){
		//get left
		if (forks[philosopherID] == 0){
			mdat_mutex_lock(&lock);
			forks[philosopherID] = 1;
			numSticks++;
		}
		
		//get right
		if (forks[(philosopherID + 1)%sizeof(forks)] == 0){
			mdat_mutex_lock(&lock);
			forks[(philosopherID + 1)%sizeof(forks)] = 1;
			numSticks++;
		}
		if (numSticks == 2){
			eat();
			forks[philosopherID] = 0;
			forks[philosopherID + 1] = 0;
			mdat_mutex_unlock(&lock);
			think();
		}
			
	}
}
