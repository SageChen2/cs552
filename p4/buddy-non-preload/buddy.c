
/**
 * Implementation of a memory allocator based on the Buddy System.
 * See Knuth Art of Computer Programming, vol. 1, page 442. 
 * Each available block starts with a header that consists
 * of a tag (free/reserved), kval (size of block 2^kval), next
 * and previous pointers. Each reserved block has the tag and kval 
 * field only. All allocations are done in powers of two. All requests
 * are rounded up to the next power of two.
 * 
 * @author  TBD
 * 
 */
 
#include "buddy.h"
#include <stdint.h>
#include <inttypes.h>




int initialized = FALSE;

/* the header for an available block */
struct block_header { // (might be wrong) a block_header struct is 24 bytes in size
	short tag;	//a short is 2 bytes
	short kval; //if kval = 3, that means this node is 2^3 = 8 bytes
	struct block_header *next;
	struct block_header *prev;
}; //(mod) need to give name for the struct block_header?
const int RESERVED = 0;
const int FREE = 1;
const int UNUSED = -1; /* useful for header nodes */


/* supports memory upto 2^(MAX_KVAL-1) (or 64 GB) in size */
#define  MAX_KVAL  37

/* default memory allocation is 512MB */
const size_t DEFAULT_MAX_MEM_SIZE = 512*1024*1024; //512MB = (512 * 1024)KB = (512 * 1024 * 1024)Bytes, 1MB=1024KB = (1024*1024)Bytes

//(mod) the maximum amount of memory that you will ever manage is limited to 32GB.
//const long long MAXIMUM_MEM_SIZE = 32 * 1024 * 1024 * 1024; //long int or size_t?

/* A static structure stores the table of pointers to the lists in the buddy system.  */
struct pool {
	void *start; // pointer to the start of the memory pool
	int lgsize;  // log2 of size
	size_t size; // size of the pool, same as 2 ^ lgsize
	/* the table of pointers to the buddy system lists */
	struct block_header avail[MAX_KVAL];
} pool;











/**
 * Initialize the buddy system to the given size 
 * (rounded up to the next power of two)
 *
 * @return  TRUE if successful, ENOMEM otherwise.
 */
int buddy_init(size_t size) { 

  
	//printf("check mark1");

	initialized = TRUE; //must

	//need to round the given size to the next power of 2
	
	if(size == 0){// might be wrong, pool.size can be over written (position of this if matters)

		pool.size = DEFAULT_MAX_MEM_SIZE;//* default memory allocation is 512MB */
		pool.lgsize = 29; //why? 512mb is 2^29 bytes
	
	}else{

	pool.lgsize = (int)ceil(log2(size));
	pool.size = pow(2,pool.lgsize);


	}

	if(pool.lgsize >= MAX_KVAL){

		return -1;

	}
	
	

	pool.start = sbrk(pool.size);

	if(pool.start == NULL){

		errno = ENOMEM;
		return errno;

	}

   //(might be wrong)do we need to actually initialze every entry of struct block_header avail[MAX_KVAL];??
	//for each array entry, assign values to avail[xx].next, avail[xx].prev, avail[xx].tag, avail[xx].kval.
	//each array entry is the head of 1 of the 37 lists
	
	int i;
	for(i = 0; i< MAX_KVAL; i++){

		pool.avail[i].tag = UNUSED; //UNUSED for head nodes
		pool.avail[i].kval = i; //(might be wrong) lgsize is for the entire memory, kval is for the actual block, kval is the power
		pool.avail[i].next = &pool.avail[i];// since at the beginning there is only 1 node, next and prev will point to the node itself
		pool.avail[i].prev = &pool.avail[i];//prev points to itself

	}
	//set up pool.avail[pool.lgsize] = pool.start, the head node at the highest kval level, why? becaue we slice the biggest memory chunk to the smallest one
	//mapping your pool to your memory, pool.avail[pool.lgsize]
	
	struct block_header * header = (struct block_header *)pool.start;
	pool.avail[pool.lgsize].next = pool.start;
	pool.avail[pool.lgsize].prev = pool.start;
	header->next = &pool.avail[pool.lgsize];
	header->prev = &pool.avail[pool.lgsize];
	header->tag = FREE;
	header->kval = pool.lgsize;


	return TRUE;
}

/**
 * Allocate dynamic memory. Rounds up the requested size to next power of two.
 * Returns a pointer that should be type casted as needed.
 * @param size  The amount of memory requested
 * @return Pointer to new block of memory of the specified size.
 */
void *buddy_malloc(size_t size)
{
		//keep in mind, there are 37 free lists, every list has chunks of free memory only  
		//set errno to ENOMEM when memory cannot be allocated

		//need to check if the size is an unresonable size, less than 1 byte
		if(size < 1){//check
			
			fprintf(stderr, "The size passed into buddy_malloc() is less than 1 byte, not valid\n");	// (size < 1 byte) is unreasonably small
			return NULL;
		}

		
		//if the sbrk() call inside buddy_init() returned -1, buddy_malloc() should return NULL, 
		if(errno == ENOMEM){//check
			//no memory can be allocated
			fprintf(stderr, "the sbrk() call inside buddy_init() returned ENOMEM, buddy_malloc() should return NULL\n");
			return NULL;
		}


		//looke at 'int initialized' to determine if buddy_init() is called directly or not
		if(initialized == FALSE){

			buddy_init(DEFAULT_MAX_MEM_SIZE); // if init() was not called explicitly

		}
		//the following might be wrong (need to check)

		//if init() was called explicitly, first thing is to round up the requested size to next power of two
		size = pow(2, ceil(log(size)/log(2))) +sizeof(struct block_header);// this size is in Bytes (checked)

		//now search the memory pool to find the appropriate block to satisfy the memory request 
		// first calculate the k value for the ‘size’ para,   k is the power. if size = 128, k = 7, cuz 2^7 = 128
		int kvalOfRequestedMemory;
		kvalOfRequestedMemory = log2(size);//Kval of my requested memory

		int i = kvalOfRequestedMemory;

		if(i > 29){

			return NULL;
		}

	
		//increment the i to go up among the 37 lists, and find the list with at least 1 free memory block
		while(( pool.avail[i].next == &pool.avail[i])  && (i != pool.lgsize)){//might be wrong, (ask) how can this be NULL, should it be pointing to itself like &pool.avail[i]?

			i++;
		}
		/**keep going up until find 1 free memory block from a list, want to allocate 2^k, but might go up to  
		the list with nodes have (2^m) in size, m>=k*/  

	

		//(might be wrong here)if already reach the last list (i = 36) and stil cannot find a free block, cant malloc, return NULL
		if((i == pool.lgsize) && (pool.avail[i].next == &pool.avail[i])){

			return NULL;
		}

		
		struct block_header * firstAvailableBlock = pool.avail[i].next;	//pool.avail[i].next is the first avaiable memory block, 'firstAvailableBlock' is the header of this block
	  // 	firstAvailableBlock->tag = RESERVED; //this first available block is no longer free, so set it to RESERVED and remove it
		
		firstAvailableBlock->next->prev = firstAvailableBlock->prev;// if A->B->C, and remove B, C's prev is set up this way
		firstAvailableBlock->prev->next = firstAvailableBlock->next;
		//int removed = removeNode(firstAvailableBlock);
		//printf("removeNode's return is: %d", removed);
		//printf("After called removeNode\n");
		

		/**
		 *  go down from current i value (the kval), go down to kval=log2(size), and add free node to the list with the requested k value(size), 
		 * 	so its tag is free, .next is null
		 *  */

		/**
		 * before the while, since now we are at a list with a higher kval, 
		 * we grab that kval and use it as condition for while loop
		 * **/
		int k = i;


		//printf("Before entering the 2nd while loop in buddy_malloc\n");
		while(k != kvalOfRequestedMemory){  //while my current level's kval is not equal the kval of the size I want to malloc, I keep spliting the left side block, and going down a level

			//printf("before k-- in 2nd while loop in buddy_malloc\n");

			k--;  
			//update kval since we are going down
			//now we need to access the right side of splited memory and added this free block back to the list
			//to access the right side (buddy), need to move forward some bytes to the header of the right side block
			struct block_header * rightSideOfSPlittedBlock = (struct block_header *)( (((char *) firstAvailableBlock) + (1ULL << k))  );// ULL is unsigned long long, (1ULL << firstAvailableBlock->kval) is 2^k

			//printf("before initializing rightSideOfSPlittedBlock in 2nd while loop in buddy_malloc\n");
			rightSideOfSPlittedBlock->tag = FREE;
			rightSideOfSPlittedBlock->next = &pool.avail[k];
			rightSideOfSPlittedBlock->prev = &pool.avail[k];
			//printf("Before pool.avail[k].next=rightSideOfSPlittedBlock;\n");
			pool.avail[k].next = rightSideOfSPlittedBlock;
			//printf("Before pool.avail[k].prev=rightSideOfSPlittedBlock;\n");
			pool.avail[k].prev=rightSideOfSPlittedBlock;
			rightSideOfSPlittedBlock->kval = k;
			//printf("Before addNode() is called in the 2nd while loop in buddy_malloc\n");
			//addNode(rightSideOfSPlittedBlock);// use the left side block (which is the firstAvailableBlock) to split, and add the right side to a list with lower kval
			//printf("After addNode() is called in the 2nd while loop in buddy_malloc\n");


			//printf("kval of firstAvailableBlock is: %d\n", firstAvailableBlock->kval);
			//printf("kval of my requested size is: %d\n", kvalOfRequestedMemory);
		
		}

		//printf("After the 2nd while loop in buddy_malloc is finished \n");
		firstAvailableBlock->tag = RESERVED;//this block is the left side of the splited free block, it will be used to split again
		firstAvailableBlock->kval = kvalOfRequestedMemory;
	return (void*)(((char *) firstAvailableBlock) + sizeof(struct block_header));
}











/**
 * Allocate and clear memory to all zeroes. Wrapper function that just calles buddy_malloc.
 *
 * @param nmemb  The number of members needed
 * @param size   Size of each member
 * @return Pointer to start of the array of members
 */
void *buddy_calloc(size_t nmemb, size_t size) 
{

	if(initialized == FALSE){

		buddy_init(0);
	}

	/**
	 * If the memory cannot be allocated, then your buddy_calloc or buddy_malloc functions 
	 * should set the errno to 
	 * ENOMEM (which is defined in errno.h header file).
	 * 
	 */
		//call buddy_malloc() and memset to 0

	size_t totalMemSize  = nmemb * size;// in bytes

	if(buddy_malloc(totalMemSize)==NULL){

		errno = ENOMEM;
		return NULL;
	}

	return memset(buddy_malloc(totalMemSize),0,totalMemSize);

}







/**
 * buddy_realloc() changes the size of the memory block pointed to by ptr to size bytes. 
 * The contents will be unchanged to the minimum of the old and new sizes; newly 
 * allocated memory will be uninitialized. If ptr is NULL, the call is equivalent 
 * to buddy_malloc(size); if size is equal to zero, the call is equivalent to buddy_free(ptr). 
 * Unless ptr is NULL, it must have been returned by an earlier call to buddy_malloc(), 
 * buddy_calloc() or buddy_realloc().
 *
 * @param  ptr Pointer to existing memory block
 * @param  size The new size of the memory block
 * @return The pointer to the resized block
 */
void *buddy_realloc(void *ptr, size_t size) 
{
	//like changing 12kb to 5kb
	//call malloc(), copy the original to new one, and free the original
	//buddy_malloc, memcopy, free.
	
	if(ptr == NULL){//if ptr is NULL, the call is equivalent to buddy_malloc(size)

		buddy_malloc(size);
		
	}
	
	if(size == 0){//if size is equal to zero, the call is equivalent to buddy_free(ptr)

		buddy_free(ptr);

	}
	
	memcpy(buddy_malloc(size),ptr, sizeof(struct block_header));
	free(ptr);
	
	return ptr;



}





/**
 * buddy_free() frees the memory space pointed to by ptr, which must have been returned 
 * by a previous call to buddy_malloc(), buddy_calloc() or buddy_realloc(). Otherwise, 
 * or if buddy_free(ptr) has already been called before, undefined behaviour occurs. If 
 * ptr is NULL, no operation is performed. 
 * @param ptr Pointer to memory block to be freed
 */
void buddy_free(void *ptr) 
{
	//free() isnt called

	//If ptr is NULL, no operation is performed. 
	if(ptr == NULL){
		
		return;
	}
	
	//move the ptr backwards first? //(ptr - header size) to move backwards
	struct block_header * ptrCast = (struct block_header*) ((char *)ptr - sizeof(struct block_header));// the address of ptr will move backwards 24 bytes
	//If buddy_free(ptr) has already been called before
	//Just check if the ptr's tag is free or not??
	if(ptrCast->tag == FREE){ 

		return;
	}
	
	struct block_header* buddy;
	void *offset;
	while(1){

			//need to find a block's buddy, using XOR
			
			//offset = (void*)(1ULL<<(ptrCast->kval)) 
			offset = (void*)((char *)ptrCast - (char *)pool.start);
			buddy = (struct block_header *)((unsigned long long)pool.start + ((unsigned long long)offset ^ (unsigned long long)(1ULL<<(ptrCast->kval))) );
			//findBuddy(ptrCast);
			//now(check 3 conditions: buddy cant be reserved, buddy is free but not the same kval, or reach top of the list)
			
			if((ptrCast->kval = pool.lgsize) || buddy->tag == RESERVED || (buddy->tag==FREE && buddy->kval != ptrCast->kval) ){ //lgsize:try to free the whole thing
		
				break;
			}
			//remove buddy node cuz it is free already
			//removeNode(buddy); // Buddy
			buddy->next->prev = buddy->prev;
			buddy->prev->next = buddy->next;
			//merging(move the ptrCast is equivalent to doubling the size)
			if(ptrCast > buddy)//switching ptrCast and buddy's positions
				ptrCast = buddy;
			ptrCast->kval++;
	}
			//add node back 
			ptrCast->next=pool.avail[ptrCast->kval].next;
			ptrCast->prev=&pool.avail[ptrCast->kval];
			pool.avail[ptrCast->kval].next=ptrCast;
			pool.avail[ptrCast->kval].next->prev=ptrCast;
			ptrCast->tag = FREE;
			//kval+1 of ptrCast
			//ptrCast->kval = ptrCast->kval + 1; 

			//if find buddy, 	you do removeNode; if cant find, add the big chunk.
			//dont change next, until merge
			//double the left side node and 
			//need to remove node from lower level


	//printf("This is the position after while loop in buddy_free is finished\n");

}


/**
 * Prints out all the lists of available blocks in the Buddy system.
 */
void printBuddyLists(void)
{
int i;
int numOfBlock = 0;;

struct block_header * header;
struct block_header * current;

//printf(" printBuddyLists\n");

for(i=0; i<MAX_KVAL; i++){

	//printf("i value in printBuddyLists is %d:\n", i);

	  header = &pool.avail[i];
	  current = header->next;
	printf("List %d: head = %p -->", i, header);
  while(header != current){
	  current = current->next;//next node
	  printf("[tag=%d,kval=%d,addr=%p] --> ", current->tag,current->kval,current);
	  numOfBlock++;

  }
  printf("<null>\n");

  

}
printf("Number of available blocks = %d", numOfBlock);



}

