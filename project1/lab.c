/**
 * Name: Jane Doe
 * Email: janedoe@u.boisestate.edu
 * Github Username: jdoe
 */
#include "lab.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>        //mod, import pthread header file





/*
 * insertion_sort(int A[], int p, int r):
 *
 * description: Sort the section of the array A[p..r].
 */
void insertion_sort(int A[], int p, int r)
{
	int j;

	for (j=p+1; j<=r; j++) {
		int key = A[j];
		int i = j-1;
		while ((i > p-1) && (A[i] > key)) {
			A[i+1] = A[i];
			i--;
		}
		A[i+1] = key;
	}
}


/*
 * serial_mergesort(int A[], int p, int r):
 *
 * description: Sort the section of the array A[p..r].
 */
void serial_mergesort(int A[], int p, int r)
{
	if (r-p+1 <= INSERTION_SORT_THRESHOLD)  {
			insertion_sort(A,p,r);
	} else {        //(mod) this is 'left side<right side' ,r is right, p is left
		int q =  r + (p - r)/2;         //(mod) how mid is calculated does not matter (left+ or right+)
		serial_mergesort(A, p, q);      //(mod) q is mid， this is sorting left-side
		serial_mergesort(A, q+1, r);    //this is sorting right-side
		merge(A, p, q, r);
	}
}

/*
 * merge(int A[], int p, int q, int r):
 *
 * description: Merge two sorted sequences A[p..q] and A[q+1..r]
 *              and place merged output back in array A. Uses extra
 *              space proportional to A[p..r].
 */
void merge(int A[], int p, int q, int r)
{
	int *B = (int *) malloc(sizeof(int) * (r-p+1));

	int i = p;
	int j = q+1;
	int k = 0;
	int l;

	// as long as both lists have unexamined elements
	// this loop keeps executing.
	while ((i <= q) && (j <= r)) {
		if (A[i] < A[j]) {
			B[k] = A[i];
			i++;
		} else {
			B[k] = A[j];
			j++;
		}
		k++;
	}

	// now only at most one list has unprocessed elements.

	if (i <= q) {
		// copy remaining elements from the first list
		for (l=i; l<=q; l++) {
			B[k] = A[l];
			k++;
		}
	} else {
		// copy remaining elements from the second list
		for (l=j; l<=r; l++) {
			B[k] = A[l];
			k++;
		}
	}

	// copy merged output from array B back to array A
	k=0;
	for (l=p; l<=r; l++) {
		A[l] = B[k];
		k++;
	}

	free(B);
}



void* parallel_mergesort(void* arg){  


	Sublist* sub = (Sublist *) arg;

  
	pthread_t threads[2]; 

	int r,p;
	int* Arr;
	

	p = sub->low;
	r = sub->high;
	Arr = sub->A;





    int q =  r + (p - r)/2;  //(mod) q is the mid point

   if(sub->currentLevel <= sub->cutoffLevel){
       

        //the recursion should stop when cutoffLevel is reached

        //need to malloc for the struct Sublist
        Sublist *subLeft = (Sublist *) malloc(sizeof(Sublist));
        Sublist *subRight = (Sublist *) malloc(sizeof(Sublist));

        //now assiagn data fields for the struct
        subLeft->A = Arr;
        subLeft->low = p;
        subLeft->high = q;
        subLeft->currentLevel =  sub->currentLevel + 1;
        subLeft->cutoffLevel = sub->cutoffLevel;

        subRight->A = Arr;
        subRight->low = q + 1;
        subRight->high = r;
        subRight->currentLevel =  sub->currentLevel + 1;
        subRight->cutoffLevel = sub->cutoffLevel;

	

		//declar an array to store struct pointers(subLeft and subRight) 
		Sublist* arrayOfSublist[2];
		arrayOfSublist[0] = subLeft;
		arrayOfSublist[1] = subRight;



		int i;
		for(i = 0; i < 2; i++){

			pthread_create(&threads[i], NULL, parallel_mergesort, arrayOfSublist[i]);//calling parallel_sort to cause recursion
	 		

		}

	

		int j;
		for(j = 0; j < 2; j++){
				pthread_join(threads[j], NULL);
		}
		



        merge(Arr, p, q, r); 

		printf("checker 6\n ");
			
        free(subLeft);
        free(subRight);


    }else{ //base-case
		
			printf("checker 7\n");
        serial_mergesort(Arr, p, r);  //left-side

			printf("checker 8\n");
	}

		

			return NULL;
}


//helper function to print the struct
void* print_struct(Sublist* sub){

		//%p for pointr
	printf("Array %p, low %d, high %d, currentLevel %d, cutoffLevel %d\n", sub->A, sub->low, sub->high, sub->currentLevel, sub->cutoffLevel);

	return NULL;
}

