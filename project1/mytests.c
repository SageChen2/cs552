#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h> /* for times system call */
#include <sys/time.h>  /* for gettimeofday system call */
#include <unistd.h>
#include <error.h>
#include "lab.h"

// function prototypes
int check_if_sorted(int A[], int n);
void generate_random_array(int A[], int n, int seed);
void serial_mergesort(int A[], int p, int r);
void merge(int A[], int p, int q, int r);
void insertion_sort(int A[], int p, int r);
double getMilliSeconds(void);
void* parallel_mergesort(void* arg); //void parallel_mergesort(int* A, int p, int r, int currentLevel, int cutoffLevel); //(mod) void parallel_mergesort(Sublist* sub);

/*
---------------------------------------------------------------------------
clock_t times(struct tms *buffer);
times() fills the structure pointed to by buffer with
time-accounting information.  The structure defined in
<sys/times.h> is as follows:
struct tms {
    clock_t tms_utime;       user time
    clock_t tms_stime;       system time
    clock_t tms_cutime;      user time, children
    clock_t tms_cstime;      system time, children
The time is given in units of 1/CLK_TCK seconds where the
value of CLK_TCK can be determined using the sysconf() function
with the agrgument _SC_CLK_TCK.
---------------------------------------------------------------------------
*/


float report_cpu_time(void)
{
	struct tms buffer;
	float cputime;

	times(&buffer);
	cputime = (buffer.tms_utime)/ (float) sysconf(_SC_CLK_TCK);
	return (cputime);
}


float report_sys_time(void)
{
	struct tms buffer;
	float systime;

	times(&buffer);
	systime = (buffer.tms_stime)/ (float) sysconf(_SC_CLK_TCK);
	return (systime);
}

double getMilliSeconds(void)
{
    struct timeval now;
    gettimeofday(&now, (struct timezone *)0);
    return (double) now.tv_sec*1000.0 + now.tv_usec/1000.0;
}


/*
 * generate_random_array(int A[], int n, int seed):
 *
 * description: Generate random integers in the range [0,RANGE]
 *              and store in A[1..n]
 */

#define RANGE 1000000

void generate_random_array(int A[], int n, int seed)
{
    int i;

	srandom(seed);
    for (i=1; i<=n; i++)
        A[i] = random()%RANGE;
}


/*
 * check_if_sorted(int A[], int n):
 *
 * description: returns TRUE if A[1..n] are sorted in nondecreasing order
 *              otherwise returns FALSE
 */

int check_if_sorted(int A[], int n)
{
	int i=0;

	for (i=1; i<n; i++) {
		if (A[i] > A[i+1]) {
			return FALSE;
		}
	}
	return TRUE;
}







int main(int argc, char **argv) {

//    update mytests.c to accept a number of levels as a command line argument.(mod)

	if (argc < 3) { // there must be at least 2 command-line argument
			fprintf(stderr, "Usage: %s <input size> <number of levels> [<seed>]\n", argv[0]); //number of levels is the cutoff level
			exit(1);
	}

	int n = atoi(argv[1]);
	int seed = 1;
    int numberOfLevels = atoi(argv[2]); //(mod) taking in the number of levels as command line argument

	if (argc == 4)//mod
		seed = atoi(argv[3]);//mod


	int *A = (int *) malloc(sizeof(int) * (n+1)); // n+1 since we are using A[1]..A[n]
	int *B = (int *) malloc(sizeof(int) * (n+1));



    Sublist * Array = (Sublist *) malloc(sizeof(Sublist));
	
	generate_random_array(A,n, seed);
	memcpy(B,A,sizeof(int)*(n+1)); //copy A's content to B

    Array->A = A;
    Array->low = 1;
    Array->high = n; //n+1? n?
    Array->currentLevel =  0; // try 0 or 1
    Array->cutoffLevel = numberOfLevels;   //
	// generate random input
	//generate_random_array(A,n, seed);

    //(mod) begin timing
	double start_timeA;
	double sorting_timeA;

	double start_timeB;
	double sorting_timeB;


	
	// sort the input (and time it)
	start_timeA = getMilliSeconds();
    parallel_mergesort(Array); 	//Array or A, might be wrong
	sorting_timeA = getMilliSeconds() - start_timeA;

	start_timeB = getMilliSeconds();
    serial_mergesort(B,1,n);
	sorting_timeB = getMilliSeconds() - start_timeB;

	// print results if correctly sorted otherwise cry foul and exit
	if (check_if_sorted(A,n)) { //1st para is Array or Array->A or A
		printf("Para Sorting %d elements took %4.2lf seconds.\n", n,  sorting_timeA/1000.0);
	} else {
		printf("%s: para sorting failed!!!!\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (check_if_sorted(B,n)) {
		printf("Serial Sorting %d elements took %4.2lf seconds.\n", n,  sorting_timeB/1000.0);
	} else {
		printf("%s: serial sorting failed!!!!\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	free(A);
	free(B);
	free(Array);//might be wrong

	exit(EXIT_SUCCESS);
}
