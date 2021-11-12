## Project 3

* Author: Zixiao Chen
* Class: CS452/CS552 [Operating Systems] Section #num


## Overview

Every device needs a driver to function. This program is a character driver called booga. This driver supports
four minor numbers, which corresponds to 4 files/partitions from booga0 to booga3. It performs open, read and write and close 
operations.Since the operations are performed in the kernel, it can be only tested on a Linux machine with access to root privilege.

## Manifest

A listing of source files and other non-generated files and a brief (one line)
explanation of the purpose of each file.

The files are as follows:

Makefile: this is file used to build the executables.

README: this file contains the documentation that describes the development process.

booga-test1.sh: this is the test script that tests the write function.

booga-test2.sh: this is the test script that tests the write and read function, and it displays the number of opens and strings output.

booga-load: this is used to load the driver.

booga-unload: this is used to unload the driver.

test-booga.c: this is a test program used to test the driver.

booga.c: this file is where the function prototypes are implemented to perform driver operations.

booga.h: this is where the booga stats is defined, along with other defined constants.


## Building the project

To build the code with the MakeFile, cd into the project folder, and run 'make', and run 'sudo ./booga_load', and then run either of the testing scripts. Note that test scripts will run 'sudo ./booga_unload'. 

## Features and usage

The most important feature is that this program is an imitation since there is no real device. Redaing from a device will produce a string and writing to a device can be reflected by the change in the booga device stats. All operations are protected by semaphores so that the correct number of bytes and other stats from read/write can be recorded into the data fields of the booga stats struct.

In addition to running the test scripts like mentioned in the above section, the test-booga.c can be manually run as './test-booga [file number] [number of bytes] [read/write]'.

For example, run './test-booga 0 100 write' should write 100 bytes to booga0.

## Testing
In addition to the testing methods mentioned in the previous sections, a user first has to confirm that the the driver is loaded. Therefore, after running 'sudo ./booga_load', run '/sbin/lsmod', and if 'booga; is displayed under 'Module', it means it is loaded.

## Known Bugs
I believe all known bugs are fixed but I will mentioned the bugs encountred in the next section.

## Reflection and Self Assessment
After reading the project description, the first thing that I think I need to spend some time on is the implementation of semaphores, because I thought I need to review on how to use a semaphore. But as I read through the example code, I quickly understood how semaphores are used here in this project.

My IDE informed me there are lots of syntax errors, and I did spend quite sometime trying to fixed them. But at some point, I realized that as long as I am able to load and unload, it wouldn't really matter.s

In order to track the number of opens and strings outputs, I added 2 extra data fields to the struct, and since there are 4 files and 4 different types of strings, the number of entries is set to 4. Some peers claimed that we do not need to do that, but I think it is counterintuitive since every different file has different stats. 

In read(), I used the example random components to index into the array that contains the 4 strings to make sure they are randomly chosen, and I used a while loop to concatenate the output strings. I had to read the instructions for __copy_to_user() to make sure I understand which parameter is the source and which is the destination, and which parameter from the read() represents the number of bytes.
When I compiled, I had a warning saying that 'ignoring return value of ‘__copy_to_user’, declared with attribute warn_unused_result'. I learned from a peer that I should declared a global int variable and assign the result of __copy_to_user() to that int. In addition, from the TA, I learned that if the returned value of __copy_to_user() is less than 0, it means that it failed to read the bytes. Semaphore is used when incrementing the number of bytes read.

In write(), the very important thing is to identify the case when trying to write to booga3. In order to track it, I declared a global int variable called 'num' and set it to the NUM(inode->i_rdev) in open(). In write(), I check if the num is 3 or not, if it is 3, I use send_sig() to terminate the process. Originally, I wanted to use send_sig_info() but I could not understand it quite well even after reading some documents, so I used send_sig(), and if my understanding is correct, 0 means that the signal has been sent by a User Mode process, current is the process itself.

When I started testing, at some point I started to have problems. When I ran test2 script many times in a row, I saw the error printed to the terminal that says 'rmmod: ERROR: Module booga is in use.' I couldn't understand why. Through TA's help, I realized that it had something to do with my semaphore positions, that some critical sessions were not protected, and therefore I adjusted to make it work.

## Sources used

https://www.kernel.org/doc/htmldocs/kernel-api/API---copy-to-user.html

https://www.halolinux.us/kernel-reference/generating-a-signal.html





