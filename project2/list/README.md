## Project 2

* Author: Zixiao Chen
* Class: CS452/CS552 [Operating Systems] Section #num


## Overview
This program creates a thread-safe version of an existing list library. The idea is to wrap an exisiting library and protect it with locks.

## Manifest

A listing of source files and other non-generated files and a brief (one line)
explanation of the purpose of each file.

- include folder: it contains .gitkeep

- lib folder: it contains .gitkeep

- list folder: this folder provides you with a pre-built doubly linked list library.

- wrapper-library: this folder contains the wrapped library implementaion

- item.c: this file implements the function prototype of item.h

- Makefile: this file is used to build and run the program

- README.md: this file contains information that will explain the project

- pc.c: this file is used to test the wrapped library implementaion, a single queue version.

- pc-mq.c: this file is used to test the wrapped library implementaion, but this one is a multi-queue version.

- test-pc.sh: this script is used to test the wrapped library implementaion (the single queue version).

- test-pc-mq.sh:  this script is used to test the wrapped library implementaion (the multi queue version).


## Building the project

To build the code, cd into the project folder, and run 'make', and then run './test-pc-mq.sh' or './test-pc.sh' to test either version of the thread safe library.



## Features and usage

Summarize the main features of your program. It is also appropriate to
instruct the user how to use your program.

The most important feature of this program is that it uses locks to lock the threads into critical section to protect them. In addition, wait() and signal() are also used so that sometimes when it is necessary, threads will sleep and release the lock, so that other threads are able to get into critical section.

## Testing

When testing the single queue version of the wrapped library implementaion, the test-pc.sh will be used. After finishing implementing all the functions, I ran the test-pc.sh, and after running the first combination of command line arguments, it was stuck. I looked into tsb_finishUp(), I realized that since producers are done, the boolean stop_requested should be set to TRUE. After that, I realized that broadcast() needs to be used in order to wake up more than one thread, and after I adjusted the argument passed into the broadcast() function, the test script passed.

When testing the multi queue version of the wrapped library implementaton, I made pc-mq.c based on pc.c, and I had multiple issues with it. It was easy to add an extra command line argument. Since the number of queues can be specified by the user, I realized that I need an array to hold all the queues. So for each queue I need to created a list. in the consumer(), I have spent quite some time to truly understand what went wrong. First, I thought about using a for loop to iterate through the array of queues, but after talking to the professor, I realized that a while loop will be better, because in a for loop, it will always increment, whereas in a while loop, I am able to control the the loop variant is incremented or not. I created while loops inside the while(TRUE) in both producer() and consumer() to check isEmpty and isFull and move the producer/consumer the round-robin style. However, I first encountered a seg fault that made the program not able to enter the innner while loop, I fixed by fixing the way that I used tsb_createList(). After that I encountered an infinite while loop, because the while loop never had a condition to break, I realized that I only need to loop the amount of times that is less than the number of queues in the array, because not all entries in the array are filled up with queues, some entries are empty. I used an if statement to check and break the loop. After this correction, I was finally able to actually run and pass the test-pc-mq.sh script.

## Known Bugs

There is no bug within the designated input range. and no memory leakage.

## Reflection and Self Assessment
The implementaion of the function prototypes in the ThreadsafeBoundedList.h is fairly easy. locks are used to protect the threads, and while loop are used to put threads into sleep when the implemented functions are not able to run. For example, while the list is empty, you actually cannot remove anything since there was nothing to be removed and therefore the threads should be put to wait(), and the threads will only wake up when the condition 'listNotEmpty' becomes true, and after the remove call, the 'listNotFull' will become ture and therefore signal() wakes up the threads that wait on that condition.

What really took a lot of time of the implementation and testing of pc-mq.c, and all my experience about it is summarized in the above 'testing' section. In retrospect, because the main work was to implement function prototypes, it is less complicated than project 1, in a way that you do not actually have to consider the interaction between each function. But testing a muli thread version does require time and focus to a certain level.

## Sources used
Other than textbook, I have not used any outside resource.