#include "ThreadsafeBoundedList.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

typedef struct tsb_list {
 struct list *list;
 int capacity;
 Boolean stop_requested;
 pthread_mutex_t mutex;
 pthread_cond_t listNotFull;
 pthread_cond_t listNotEmpty;
}TSB_List;

/**
 * Constructor: Allocates a new listist object and initializes its members. Initialize
 * the mutex and condition variables associated with the bounded list monitor. Set the
 * capacity of the list.
 *
 * @return a pointer to the allocated list.
 */
struct tsb_list * tsb_createList(int (*equals)(const void *, const void *),     // int (*equals)(const void *, const void *), a function pointer?
                   char * (*toString)(const void *),
                   void (*freeObject)(void *),
				   int capacity){

//malloc
TSB_List * list = (TSB_List*)malloc(sizeof(TSB_List));
//assigning data fields
list -> list = createList(equals, toString, freeObject);
list->capacity = capacity;
list->stop_requested = FALSE;

//initialize mutex and condition variables
pthread_mutex_init(&list->mutex, NULL);
pthread_cond_init(&list->listNotEmpty,NULL);
pthread_cond_init(&list->listNotFull, NULL);

return list;
 }//check

 /**
 * Frees all elements of the given list and the <code>struct *list</code> itself.
 * Does nothing if list is <code>NULL</code>. Also frees the associated mutex and
 * condition varibles and the wrapper structure.
 *
 * @param list a pointer to a <code>List</code>.
 */
void tsb_freeList(struct tsb_list * list){

// //Does nothing if list is <code>NULL</code>
// if(list == NULL){

//         return;
// }

pthread_mutex_lock(&list->mutex);//lock for criticial section
freeList(list->list);
pthread_mutex_unlock(&list->mutex);

    
} /* destructor */


/**
 * Returns the size of the given list.
 *
 * @param list a pointer to a <code>List</code>.
 * @return The current size of the list.
 */
int tsb_getSize(struct tsb_list * list){


    int size = 0;

    pthread_mutex_lock(&list->mutex);  //get into lock
    size = getSize(list->list);
    pthread_mutex_unlock(&list->mutex);   //unlock
    
    return size;

}


/**
 * Returns the maximum capacity of the given list.
 *
 * @param list a pointer to a <code>List</code>.
 * @return The macimum capacity of the list.
 */
int tsb_getCapacity(struct tsb_list * list){

        int capacity = 0;

        pthread_mutex_lock(&list->mutex);
        capacity = list->capacity;
        pthread_mutex_unlock(&list->mutex);
        return capacity;

}


/**
 * Sets the maximum capacity of the given list.
 *
 * @param list a pointer to a <code>List</code>.
 * @param capacity the maximum allowed capacity of the list
 * @return none
 */
void tsb_setCapacity(struct tsb_list * list, int capacity){

        pthread_mutex_lock(&list->mutex);
        list->capacity = capacity;      //set list's capacity to the argument passed in
        pthread_mutex_unlock(&list->mutex);



}



/**
 * Checks if the list is empty.
 *
 * @param  list a pointer to a <code>List</code>.
 * @return true if the list is empty; false otherwise.
 */
Boolean tsb_isEmpty(struct tsb_list * list){

        //lock already called in tsb_getSiz()
        int size = tsb_getSize(list);

        //check condition
        if(size == 0){
                return TRUE;
        }
        return FALSE;

}



/**
 * Checks if the list is full.
 *
 * @param  list a pointer to a <code>List</code>.
 * @return true if the list is full to capacity; false otherwise.
 */
Boolean tsb_isFull(struct tsb_list * list){

        //might be wrong?
        int size = tsb_getSize(list);
        int maximumCapacity = tsb_getCapacity(list);

        if(size == maximumCapacity){

                return TRUE;

        }
        return FALSE;



}


/**
 * Adds a node to the front of the list. After this method is called,
 * the given node will be the head of the list. (Node must be allocated
 * before it is passed to this function.) If the list and/or node are NULL,
 * the function will do nothing and return.
 *
 * @param list a pointer to a <code>List</code>.
 * @param node a pointer to the node to add.
 */
void tsb_addAtFront(struct tsb_list * list, NodePtr node){

        // //if null, do nothing
        // if(list == NULL || node == NULL){

        //       return;  

        // }

        pthread_mutex_lock(&list->mutex);//lock

        //when the while condition is true, nothing can be added to the list
        while(list->list->size >= list->capacity){//Sleep while size > maximum capacity, because there is no room to add another node

                //&list->listNotFull is the Address of the condition variable to wait on
                //&list->mutex is the Address of the mutex associated with the condition variable
                //The responsibility of wait() is to release the lock and put the calling thread to sleep (atomically); (textbook)
                //the thread is waiting on the condition 'listNotFull', the thread sleeps until 'listNotFull' becomes true
                pthread_cond_wait(&list->listNotFull,&list->mutex);//realease the lock and put the calling thread to sleep(atomically)


        }
        
        //if the capacity is not reached/full, the original function from List.h
        addAtFront(list->list, node);

        pthread_cond_signal(&list->listNotEmpty);//send signal to wake a sleeping thread waiting on the condition called 'listNotEmpty' to be become true, because now 'listNotEmpty' just became true, so the sleeping thread can wake up
        pthread_mutex_unlock(&list->mutex);

}




/**
 * Adds a node to the rear of the list. After this method is called,
 * the given node will be the tail of the list. (Node must be allocated
 * before it is passed to this function.) If the list and/or node are NULL,
 * the function will do nothing and return.
 *
 * @param list a pointer to a <code>List</code>.
 * @param node a pointer to the node to add.
 */
void tsb_addAtRear(struct tsb_list * list, NodePtr node){


        // //if null, do nothing
        // if(list == NULL || node == NULL){

        //       return;  

        // }

      pthread_mutex_lock(&list->mutex);//lock


  //when the while condition is true, nothing can be added to the list
        while(list->list->size >= list->capacity){//Sleep while size > maximum capacity, because there is no room to add another node

                //&list->listNotFull is the Address of the condition variable to wait on
                //&list->mutex is the Address of the mutex associated with the condition variable
                //The responsibility of wait() is to release the lock and put the calling thread to sleep (atomically); (textbook)
                pthread_cond_wait(&list->listNotFull,&list->mutex);//realease the lock and put the calling thread to sleep(atomically)


        }

        addAtRear(list->list, node);

        pthread_cond_signal(&list->listNotEmpty);//send signal to wake a sleeping thread waiting on the condition called 'listNotEmpty' to be become true, because now 'listNotEmpty' just became true, so the sleeping thread can wake up
        pthread_mutex_unlock(&list->mutex);



}


/**
 * Removes the node from the front of the list (the head node) and returns
 * a pointer to the node that was removed. If the list is NULL or empty,
 * the function will do nothing and return NULL.
 *
 * @param list a pointer to a <code>List</code>.
 * @return a pointer to the node that was removed.
 */
NodePtr tsb_removeFront(struct tsb_list * list){


        //  //if null, do nothing
        // if(list == NULL || tsb_getSize(list) == 0){

        //       return;  

        // }


    pthread_mutex_lock(&list->mutex);//lock

   //cannot remove if the list is empty
    while((list->stop_requested == FALSE) && (list->list->size == 0) ){

             //&list->listNotEmpty is the Address of the condition variable to wait on
            //&list->mutex is the Address of the mutex associated with the condition variable
            //The responsibility of wait() is to release the lock and put the calling thread to sleep (atomically); (textbook)
            //the thread sleeps until 'listNotEmpty' becomes true
            pthread_cond_wait(&list->listNotEmpty, &list->mutex); 

    }

        NodePtr removedNode = removeFront(list->list);

        pthread_cond_signal(&list->listNotFull);// after removing a node, listNotFull becomes true
        pthread_mutex_unlock(&list->mutex);

        return removedNode;

}

/**
 * Removes the node from the rear of the list (the tail node) and returns
 * a pointer to the node that was removed. If the list is NULL or empty,
 * the function will do nothing and return NULL.
 *
 * @param list a pointer to a <code>List</code>.
 * @return a pointer to the node that was removed.
 */
NodePtr tsb_removeRear(struct tsb_list * list){

        // //if null, do nothing
        // if(list == NULL || tsb_getSize(list) == 0){

        //       return;  

        // }

        pthread_mutex_lock(&list->mutex);//lock

    //cannot remove if the list is empty
    while((list->stop_requested == FALSE) && (list->list->size == 0) ){

             //&list->listNotEmpty is the Address of the condition variable to wait on
            //&list->mutex is the Address of the mutex associated with the condition variable
            //The responsibility of wait() is to release the lock and put the calling thread to sleep (atomically); (textbook)
            //the thread sleeps until 'listNotEmpty' becomes true
            pthread_cond_wait(&list->listNotEmpty, &list->mutex); 

    }

        NodePtr removedNode = removeRear(list->list);

        pthread_cond_signal(&list->listNotFull);// after removing a node, listNotFull becomes true, so the threads that wait for it to become true will wake up.
        pthread_mutex_unlock(&list->mutex);

        return removedNode;

}


/**
 * Removes the node pointed to by the given node pointer from the list and returns
 * the pointer to it. Assumes that the node is a valid node in the list. If the node
 * pointer is NULL, the function will do nothing and return NULL.
 *
 * @param list a pointer to a <code>List</code>.
 * @param node a pointer to the node to remove.
 * @return a pointer to the node that was removed.
 */
NodePtr tsb_removeNode(struct tsb_list * list, NodePtr node){

        // //check null
        // if(node == NULL){

        //         return;

        // }

        pthread_mutex_lock(&list->mutex);//lock

         while((list->stop_requested == FALSE) && (list->list->size == 0) ){

          pthread_cond_wait(&list->listNotEmpty, &list->mutex); 


         }

         NodePtr removedNode = removeNode(list->list, node);


        pthread_cond_signal(&list->listNotFull);// after removing a node, listNotFull becomes true, so the threads that wait for it to become true will wake up.
        pthread_mutex_unlock(&list->mutex);

        return removedNode;

}


/**
 * Searches the list for a node with the given key and returns the pointer to the
 * found node.
 *
 * @param list a pointer to a <code>List</code>.
 * @param the object to search for.
 * @return a pointer to the node that was found. Or <code>NULL</code> if a node 
 * with the given key is not found or the list is <code>NULL</code> or empty.
 */
NodePtr tsb_search(struct tsb_list * list, const void *obj){

        // //if the list is null or empty
        // if(list == NULL || tsb_getSize(list) == 0){
        //         return;
        // }


        pthread_mutex_lock(&list->mutex);//lock


        //passing the while loop means that the list is not empty, and therefore search begins
        NodePtr resultNode = search(list->list, obj);

        pthread_mutex_unlock(&list->mutex);

        return resultNode;
}



/**
 * Reverses the order of the given list.
 *
 * @param list a pointer to a <code>List</code>.
 */
void tsb_reverseList(struct tsb_list *  list){

        pthread_mutex_lock(&list->mutex);//lock
        reverseList(list->list);
        pthread_mutex_unlock(&list->mutex);


}

/**
 * Prints the list.
 *
 * @param list a pointer to a <code>List</code>.
 */
void tsb_printList(struct tsb_list * list){

      pthread_mutex_lock(&list->mutex);//lock
      printList(list->list);
      pthread_mutex_unlock(&list->mutex);

}

/**
 * Finish up the monitor by broadcasting to all waiting threads
 */
void tsb_finishUp(struct tsb_list * list){

        //When stop_requested is set to true, consumers stop consuming/waiting
        list->stop_requested = TRUE;
        //to ensure all threads wake up
        //broadcast(&list->listNotEmpty) wake ups all threads that are waiting for 'listNotEmpty' to beceome true.
        pthread_cond_broadcast(&list->listNotEmpty);
        //pthread_cond_broadcast(&list->listNotFull);
}