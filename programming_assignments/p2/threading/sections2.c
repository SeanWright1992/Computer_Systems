//Sean Wright
//cpsc 3500
//sections1.c: mutual exclusion model sections

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include "sections.h"
#include "mdat.h"


pthread_mutex_t lock;
int* forks;
int totalPhilo;

void
sectionInitGlobals(int numPhilosophers)
{
	const char* TRACE = "Lock trace";
	int i;
	
	mdat_mutex_init(TRACE, &lock, NULL);
	forks = (int*) malloc(numPhilosophers* sizeof(int));
	for(i = 0; i < numPhilosophers; i++)
		forks[i] = 0;
	totalPhilo = numPhilosophers;
}

void
sectionRunPhilosopher(int philosopherID, int numRounds)
{
	int i = 0;
	int numSticks = 0;
	int eaten; 
	
	while (i < numRounds){
		//the highest num philosopher gets highest num chopstick
		if (philosopherID == (totalPhilo-1)){
			mdat_mutex_lock(&lock);
			forks[philosopherID] = 1;
			numSticks++;
			mdat_mutex_unlock(&lock);
		} else { //otherwise pick up lowest num stick of two options if avilable
			if (forks[philosopherID] == 0){
				mdat_mutex_lock(&lock);
				forks[philosopherID] = 1;
				numSticks++;
				mdat_mutex_unlock(&lock);
			}
		}
		//then try and grab highest of two sticks or 0 for highest philosopher
		if (philosopherID == (totalPhilo-1)){
			mdat_mutex_lock(&lock);
			forks[0] = 1;
			numSticks++;
			mdat_mutex_unlock(&lock);
		} else {
			if(numSticks==1){
				mdat_mutex_lock(&lock);
				if (forks[philosopherID + 1] == 0){
					forks[philosopherID + 1] = 1;
					numSticks++;
					
				}
				else{
					if(numSticks ==1){
						forks[philosopherID] = 0;
						numSticks--;
					}
				}
				mdat_mutex_unlock(&lock);
			}
		}
		
		if (numSticks == 2){
			eat();
			eaten = 1;
			mdat_mutex_lock(&lock);
			forks[philosopherID] = 0;
			forks[(philosopherID + 1)%totalPhilo] = 0;
			numSticks = 0;
			mdat_mutex_unlock(&lock);
		}
		
		if (eaten == 1){
			think();
			eaten = 0;
			i++;
		}
			
	}
}
