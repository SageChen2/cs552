## Project 4

* Author: Zixiao
* Class: CS452/CS552 [Operating Systems] Section #num

## Overview

This progam is running a memory managment system, the buddy system. In this system, there is a pool of
free memory blocks that reside in 37 doubly-linked circular lists, and every free block is of the size 2^kval.
The user is able to allocate/free memory, and the default pool size is  512MB.


## Manifest

A listing of source files and other non-generated files and a brief (one line)
explanation of the purpose of each file.

The folder of buddy-non-preload contains the following files:

Makefile - used to build the program

buddy-test.c - this is 1 the 3 test programs

buddy-unit-test.c - this is 1 the 3 test programs

malloc-test.c - this is 1 the 3 test programs

buddy.c - this is where each related function is implemented.

buddy.h - this is where each related function is declared and documented.


The folder of buddy-preload contains the following files:

Makefile - used to build the program

malloc-test.c - this is 1 the 3 test programs

buddy.c - this is where each related function is implemented.

buddy.h - this is where each related function is declared and documented.

## Building the project

Simply build the code by running 'make all' and it will execute all related commands specified in the Makefile.


## Features and usage

The main feature is that this is a buddy system. Orignially at the start of the memory pool, there is 1 free block of the size 512MB. When a user request some memory, the size of the requested memory will be rounded to the nearest power of 2, and when the search for free block begins, the only 1 free buddy of 512MB will be recursively divded by 2, until a suitable free block is found.

User simply uses buddy_malloc() to request memory (before interposing), and use buddy_free() to free memory (before interposing), and buddy_free() is called, coalescing will happen, which means that the freed memory block will start to merge with its buddy.

There are 37 lists, with kval from 0 to 36, but because we can only manage no more than 512MB, the biggest reacheable memory size is only up to the kval = 29.

## Testing

This section should detail how you tested your code. Simply stating "I ran
it a few times and it seems to work" is not sufficient. Your testing needs to
be detailed here.

There are 3 provided tests to be tested. I for sure passed buddy-unit-test which consists of a simple free test and a max allocation test, so that my program can allocate/free 1 byte, and it can allocate a block with a size that is below certain amount (the max).

I run malloc-test with the following arguments : ./malloc-test 10 1 i , and various other combination of arguments, and judging by the non-error exits, I think it passed the test.

However, I failed buddy-test and got a seg fault. With the help of my peer Nina, I printed out the virtual address of some of the blocks in my list, and strange thing is that the block that should have a starting virtuall address of all 0s, did not start with all 0s. I honestly do not know which part to look into for debugging, may be something wrong with the offset. and with the limited time I am afraid this is where I stop, without interposing.

## Known Bugs
Seg fault on running buddy-test.


## Reflection and Self Assessment

Buddy system is mentioned in chap17 of the textbook, only a handful of paragraphs. The concept is clear to me, but the actual implementation with various details really overwhelmed me.

First thing first, due to the nature of the memory pool, the proper implmentation should be a type of linked lists. I was very clear that my implementation is circular, because in my buddy_init() when I init all the head nodes of the 37 lists, I set the pointers pointing back to itself, since free blocks are not in the list yet. In addition, the head node at the list with the highest kval should be initialized, because this is the initial free space that will be divded.

In buddy_malloc(), when we allocate memory, we first go up the lists to find at least 1 free block, and divide this block until it becomes the size that we requested. Concept is clear, and the implementation is clear: I used 2 while loops to go up/down. I add the buddy back to the list with a lower kval since the size is already half of the original. I eventually return the available block that is on the left side visually. plus the header size, because if without adding the header size,  the returned free memeory will not be exactly what was requested since the header does take some space. 

At some point, I mixed circular list with regular doubly-linked list, because TA taught us according to her implementation, and I was lost and did not stay consistent with my own implementation so that it took quite sometime to adjust the pointers, with the help of my professor and peers.

In buddy_free(), my implementation uses a while loop to find buddy and merge, it adds the node back to the list and mark it as free after the while loop is broken by 1 of the 3 special conditions. The offset between the memory block to be freed and the start of the memory pool is calculated.

There are various bugs orginally, because of my inconsistency with implementation, and the lack of ability to organize the logic being used, which means that a lot of things that should be in the right place did not happen in the right place.

In retrospect, my biggest issue with such a project, is that I lack the ability to make a plan and stick to the plan and verify each step. I also lack the ability to put what I learned from the textbook into detailed program, even though I know the concept fairly well. A project of this volume like this exposed my problem.

However, learning is a process and it is on-going, as I spend more time on it, I will be able to learn more and do what I was not able to do, at some point in the future.

