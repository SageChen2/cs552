#ifndef __LAB_H
#define __LAB_H

#ifdef __cplusplus
extern "C" {
#endif

#define TRUE 1
#define FALSE 0



//Create a struct to store Array, low and high index in the A[]
typedef struct Sublist_struct{

        int* A; //pointer to the A[] array in serial_mergesort()
        int low;//this is p in A[p...r]
        int high;//this is r in A[p..r]
        int currentLevel;//current level "currentLevel"
        int cutoffLevel;
}Sublist;


// function prototypes
void serial_mergesort(int A[], int p, int r);
void merge(int A[], int p, int q, int r);
void insertion_sort(int A[], int p, int r);
void* parallel_mergesort(void* arg);
void* print_struct(Sublist* sub);
// void* thread_mergesort(void* arg);





const int INSERTION_SORT_THRESHOLD = 100; //based on trial and error

#ifdef __cplusplus
}
#endif
#endif //__LAB_H
