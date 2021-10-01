
## Project 1

* Author: Zixiao Chen
* Class: CS452/CS552 [Operating Systems] Section #num

## Overview


This program implements the mergsort by using mutiple threads from the pthread library. The program will limit the number of levels it uses via a command line argument. The number of levels supplied by user is the cut off level, when it is reached, no more levels will be created.

## Manifest

A listing of source files and other non-generated files and a brief (one line)
explanation of the purpose of each file.

- example folder: this folder contains the example pictures for data analysis.

- Makefile: this is file used to build the executables.

- README: this file contains the documentation that describes the development process.

- lab.c: this file contains all the sorting/merging functions

- lab.h: this file contains all the function prototypes along with the struct

- mytest.c: this file contains the main function which runs the parallel sorting and serial sorting.

- runval.sh: this file runs the valgrind to check memeory leakage.


## Building the project

To build the code, cd into the project folder, and run 'make', and then run './mytests' with input size n and number of levels

## Features and usage

Summarize the main features of your program. It is also appropriate to
instruct the user how to use your program.

The one of the important main features of this program is that it uses a pointer to the struct used, instead of instantiating the struct itself.

To use the program, after 'make', just run './mytests' with input size n and number of levels, the seed is optional.

For example, run './mytests 100000000 4' .

## Testing

Testing/debugging did require some signifcant work. After finishing the coding, I ran 'make', but I had some errors. For example, I passed in the wrong type of argument for the parallel_mergesort() function passed in the pthread_create(), it takes in a void pointer, which can be casted into any type of pointer, but at some point I passed in a struct and it should be a struct pointer.

After quite some time spent on testing, I was able to run 'make' without getting errors/warnings, but i got segmentation fault(core dump), so I checked all the malloc() and free(), and somehow I realized that I did not use malloc for certain sturct pointers used(I changed and tested many times so I forgot the details).


## Known Bugs

There is no bug within the designated input range. and no memory leakage.

## Reflection and Self Assessment

There are many issues I encountered during development and testing. At first I had to make the chioce between using number of threads or number of levels. I was using threads, not realizing that the number of threads is really hard to be tracked as the level goes deep. After using the professor's office hour, I switched to using levels. The mechanism of level is that once the current level reaches commandline-entered 'number of level', the new thread generation will be totally cut off.

I had to research the pthread_create() function and the arguments passed in because I didn't quite understand the textbook's explanation. Since I am calling parallel_mergesort() function in the pthread_create() by itself to cause recursion, the argument I passed in has to be a void pointer, and in fact it is the pointer that points to  the struct I created.

At a certain point, I had segmentation fault (core dump) not caused by malloc() and free(). I created a function to print out all 5 data fields of my Sublist struct, only to find out that after the first recursion, the low and high indices of the struct, along with the currentLevel and cutoffLevel, the 4 paramemters grew to very large numbers, and I thought this was the reason for the segmentation fault. I was using 2 calls of pthread_create() and 2 calls of pthread_join(), and I realized that if I call pthread_create() twice, one after another, the recursion might have some serious issue.


So I created an array of threads and an array of structs to store my 2 threads that will be created and the 2 structs that represent the left side and right side of a split array. I used 2 for loops, 1 for creating the threads and the other for joing the threads. Finally, these 2 for loops made my program to run. Even though for loops are used, but my pthread_create() is still calling the parallel_mergsort() to cause recursion.


## Sources used
Other than the textbook, I searched the description of pthread_create() to  make sure I pass in the correct types of arguments. I found it on a CMU website:
https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html