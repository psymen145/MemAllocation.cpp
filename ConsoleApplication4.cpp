#include "stdafx.h"
#include <iostream>
#include <list>
#include <fstream>

using namespace System;
using namespace std;

//how each memory block is represented
struct MemBlock{
	MemBlock(int blockSize) :size(blockSize),processID(0) {};
	int processID;
	int size;
};

//function declarations
MemBlock* bestFitAllocate(int pid, int req_size, list<MemBlock>& mem_map);
MemBlock* firstFitAllocate(int pid, int req_size, list<MemBlock>& mem_map);
MemBlock* worstFitAllocate(int pid, int req_size, list<MemBlock>& mem_map);
MemBlock* nextFitAllocate(int pid, int req_size, list<MemBlock>& mem_map, MemBlock* mostRecent);
void releaseMemory(MemBlock* desiredRemoved, list<MemBlock>& mem_map);

int main(){
	int blocks;
	int blocksize;
	list<MemBlock>mem_map;

	cout << "How many blocks are there: " << endl;
	cin >> blocks;

	//initializes the block size for each block in the memory map
	for (int i = 1; i <= blocks; i++) {
		cout << "What is the size of " << i << " block: " << endl;
		cin >> blocksize;
		mem_map.push_back(MemBlock(blocksize));
	}
	
	/************************************
					TESTING
	************************************/
	//10 test case requests
	//first parameter is PROCESS ID, second is REQUEST SIZE, third is MEMORY MAP
	firstFitAllocate(111, 128, mem_map);
	worstFitAllocate(222, 55, mem_map);
	MemBlock* p = bestFitAllocate(333, 50, mem_map);

	if (p != NULL) {	//if p returned NULL, that means there isn't enough space in the map
						//thus these functions won't work
		nextFitAllocate(444, 100, mem_map, p);
		releaseMemory(p, mem_map);
	}

	firstFitAllocate(555, 100, mem_map);
	bestFitAllocate(777, 10, mem_map);
	p = worstFitAllocate(666, 210, mem_map);

	if(p != NULL){
		nextFitAllocate(888, 20, mem_map, p);
		releaseMemory(p, mem_map);
	}

	/*************************************
					OUTPUT
	*************************************/

	//show memory map and processid and size of each block
	int blockNum = 1;

	cout << endl;
	cout << "PARTITION\t" << "PROCESS_ID\t" << "SIZE" << endl;

	for (list<MemBlock>::const_iterator it = mem_map.begin(); it != mem_map.end(); it++) {
		cout << blockNum << "\t\t" << (*it).processID << "\t\t" << (*it).size << endl;
		blockNum++;
	}

	cout << endl;

    return 0;
}

MemBlock* bestFitAllocate(int pid, int req_size, list<MemBlock>& mem_map) {
	int bestBlockPosition = 0;	//keeps track of the iterations of the bestBlockSize so far
	int counter = 0;	//keeps track of how many iterations we did in the loop
	int bestBlockSize = numeric_limits<int>::max();	//sets the best block size to the biggest num for int
	MemBlock* MemBlockPointer;

	//loop through list to see which block is the best fit
	for (list<MemBlock>::const_iterator it = mem_map.begin(); it != mem_map.end(); it++) {

		counter++;

		if ((*it).processID == 0 && (*it).size >= req_size) {	//we have to check if the block isn't already taken by another process
																//And only consider blocks that are big enough
			if ((*it).size < bestBlockSize) {
				bestBlockPosition = counter;		//change the position to where best so far is at
				bestBlockSize = (*it).size;		//change the bestSize so far
			}
		}
	}

	counter = 0;

	//now go to the iterate to the position and change the processID
	for (list<MemBlock>::iterator it = mem_map.begin(); it != mem_map.end(); it++) {

		counter++;

		if (bestBlockPosition == counter) {
			if ((*it).size == req_size) {		//if best block is a perfect match	
				(*it).processID = pid;
				MemBlockPointer = &(*it);
				return MemBlockPointer;
			}
			else if ((*it).size > req_size) {
				(*it).processID = pid;
				mem_map.insert(it, MemBlock((*it).size - req_size));	//creates another block that's the size of 
																		//what's remaining after the process occupies the block
				(*it).size = req_size;			//change the process's block to the required size
				MemBlockPointer = &(*it);
				return MemBlockPointer;
			}
		}
	}

	//if we reach the end of the function, that means no blocks were big enough to fit the process' request
	return NULL;
}

MemBlock* firstFitAllocate(int pid, int req_size, list<MemBlock>& mem_map) {
	MemBlock* MemBlockPointer;

	//loop through list to see first block that fits the request size
	for (list<MemBlock>::iterator it = mem_map.begin(); it != mem_map.end(); it++) {
		if ((*it).processID == 0) {				//we have to check if the block isn't already taken by another process
			if ((*it).size == req_size) {		//if it is a perfect match	
				(*it).processID = pid;
				MemBlockPointer = &(*it);
				return MemBlockPointer;
			}
			else if ((*it).size > req_size) {	//if the block is bigger than the requested size
				(*it).processID = pid;
				mem_map.insert(it, MemBlock((*it).size - req_size));	//creates another block that's the size of 
																		//what's remaining after the process occupies the block
				(*it).size = req_size;			//change the process's block to the required size
				MemBlockPointer = &(*it);
				return MemBlockPointer;
			}
		}
	}

	//if we reach the end of the loop, that means no blocks were big enough to fit the process' request
	return NULL;
}


MemBlock* worstFitAllocate(int pid, int req_size, list<MemBlock>& mem_map) {
	int biggestBlockPosition = 0;	//keeps track of the iterations of the biggestBlockSize so far
	int counter = 0;	//keeps track of how many iterations we did in the loop
	int biggestBlockSize = 0;
	MemBlock* MemBlockPointer;

	//loop through list to see which block is the biggest
	for (list<MemBlock>::const_iterator it = mem_map.begin(); it != mem_map.end(); it++) {

		counter++;

		if ((*it).processID == 0 && (*it).size >= req_size) {	//we have to check if the block isn't already taken by another process
																//And only consider blocks that are big enough
			if ((*it).size > biggestBlockSize) {
				biggestBlockPosition = counter;		//change the position to where biggest so far is at
				biggestBlockSize = (*it).size;		//change the biggestSize so far
			}
		}
	}

	counter = 0;

	//now go to the iterate to the position and change the processID
	for (list<MemBlock>::iterator it = mem_map.begin(); it != mem_map.end(); it++) {

		counter++;

		if (biggestBlockPosition == counter) {
			if ((*it).size == req_size) {		//if biggest block is still a perfect match	
				(*it).processID = pid;
				MemBlockPointer = &(*it);
				return MemBlockPointer;
			}
			else if ((*it).size > req_size) {
				(*it).processID = pid;
				mem_map.insert(it, MemBlock((*it).size - req_size));	//creates another block that's the size of 
																		//what's remaining after the process occupies the block
				(*it).size = req_size;			//change the process's block to the required size
				MemBlockPointer = &(*it);
				return MemBlockPointer;
			}
		}
	}

	//if we reach the end of the function, that means no blocks were big enough to fit the process' request
	return NULL;
}

MemBlock* nextFitAllocate(int pid, int req_size, list<MemBlock>& mem_map, MemBlock* mostRecent) {
	int recentPID = (*mostRecent).processID;	//locate where to start allocating from
	MemBlock* MemBlockPointer;
	list<MemBlock>::iterator it2 = mem_map.end();	//we keep track of the firstBlock that fits BEFORE the recentPID
												//we do this so we do not need to iterate through the list a second time
												//if a block isn't found after the recentPID
	int flag = 0;	//used to prevent it2 from constantly changing iterator
	int flagBeginPoint = 0;		//where last allocation was located

	for (list<MemBlock>::iterator it = mem_map.begin(); it != mem_map.end(); it++) {

		if ( (*it).size >= req_size && (*it).processID == 0 && flag == 0 ) { //keep track of first free space before recentPID
			it2 = it;
			flag = 1;
		}
		if ((*it).processID == recentPID) {	//starting point
			flagBeginPoint = 1;
		}
		//we know because of flagBeginPoint, that nextFit begins here
		if (flagBeginPoint == 1 && (*it).size >= req_size && (*it).processID == 0 ){
			if ((*it).size == req_size) {		//if it is a perfect match	
				(*it).processID = pid;
				MemBlockPointer = &(*it);
				return MemBlockPointer;
			}
			else if ((*it).size > req_size) {	//if the block is bigger than the requested size
				(*it).processID = pid;
				mem_map.insert(it, MemBlock((*it).size - req_size));	//creates another block that's the size of 
																		//what's remaining after the process occupies the block
				(*it).size = req_size;			//change the process's block to the required size
				MemBlockPointer = &(*it);
				return MemBlockPointer;
			}
		}
	}

	//only executes if block isn't found after the most recent allocated.
	if (it2 != mem_map.end()) {
		if ((*it2).size == req_size) {		//if it is a perfect match	
			(*it2).processID = pid;
			MemBlockPointer = &(*it2);
			return MemBlockPointer;
		}
		else if ((*it2).size > req_size) {	//if the block is bigger than the requested size
			(*it2).processID = pid;
			mem_map.insert(it2, MemBlock((*it2).size - req_size));	//creates another block that's the size of 
																	//what's remaining after the process occupies the block
			(*it2).size = req_size;			//change the process's block to the required size
			MemBlockPointer = &(*it2);
			return MemBlockPointer;
		}
	}

	//if we reach the end of the function, that means no blocks were big enough to fit the process' request
	return NULL;
}

void releaseMemory(MemBlock* desiredRemoved, list<MemBlock>& mem_map) {
	int desiredRemPID = (*desiredRemoved).processID;
	int sizeOfRemoved = (*desiredRemoved).size;
	list<MemBlock>::iterator it2 = mem_map.end();	//stores location after block has been removed
													//we need this because we have to break out of loop once deleted

	//first we have to find where the block is on the list
	for (list<MemBlock>::iterator it = mem_map.begin(); it != mem_map.end(); it++) {
		if ((*it).processID == desiredRemPID) {
			it = mem_map.erase(it);
			it2 = --it;	//we have to decrement iterator once so it points to block right before the deleted block
							//if we do not do this, the iterator is pointing to the block that is the after the deleted block
			break;
		}
	}

	(*it2).size += sizeOfRemoved;
}