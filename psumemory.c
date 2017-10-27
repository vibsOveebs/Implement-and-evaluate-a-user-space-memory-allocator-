#include <assert.h>
#include "psumemory.h"
#include <sys/mman.h>
#include <time.h>
#include <unistd.h> 
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
#define MAGICVAL 123456   /* Referenced in OSTEP - CH17 pg6   */ 
node_t* head;    /* List head - default is NULL      */
int fit;         /* Algorithm for finding free space */
int headSize, nodeSize;   /* Quick reference */
struct timespec s_begin;  /* For timing metrics */
struct timespec s_end;    /* For timing metrics */
FILE *fp;                 /* A pointer for file operations */

/* Initialize the variables */
int psumeminit(int algo, int sizeOfRegion)
{   
    headSize = sizeof(header_t); 
    nodeSize = sizeof(node_t);
    head = NULL;       /* Default value                       */
    fit = algo;        /* Select the allocation algorithm     */
    
    /* Referenced in OSTEP - CH17 pg6 */      
    head = mmap(NULL, sizeOfRegion, PROT_READ | PROT_WRITE,MAP_ANON | MAP_PRIVATE, -1, 0);
    if(head == NULL || (algo != 0 && algo != 1)) /* Not enough free space, return error */
        return -1;                               /* OR bad algorithm assignment..       */
    /* Setup free list and structure */
    head->size = sizeOfRegion - nodeSize;
    head->next = NULL;
    return 0;
}

/* Allocate memory of SIZE and return a pointer to the first *
 * location. If the memory cannot be allocated, return NULL. */
void* psumalloc(int size) {
    /* Measure the performance of this function */
    fp = fopen("measure_malloc.txt", "a+");
    clock_gettime(CLOCK_MONOTONIC, &s_begin);
    
    int leftOver;
    node_t *newP = NULL, *oldP = NULL;

    /* Referenced in OSTEP - CH17 pg6                             *
     * find free location of size (include size of tracking info) */
    const int deltaSize = nodeSize - headSize;  

    /* leftOver refers to leftover space */
    if(fit == 0)         /* Best-fit     */   
        leftOver = INT_MAX;
    else if(fit == 1)    /* Worst-fit    */
        leftOver = 0;
    else return NULL;    /* No algorithm */

    /* Used for measuring faults */
    int totalFree = 0;

    /* Temporary pointers for locating the chunk */
    node_t *newTP = NULL, *oldTP = NULL;
    /* Search for the appropriate chunk */
    for(node_t* ptr = head; ptr != NULL; ptr = ptr->next) {

        if(((header_t*)ptr)->magic != MAGICVAL) totalFree += ptr->size; /* Calculate free space */
        
	/* Calculate the leftover size for this chunk */     
        int remSize = ((ptr->size)-size) + deltaSize;
	if((fit == 0 && remSize >= 0 && remSize <= leftOver) || (fit == 1 && remSize > leftOver)) {
	    /* New fit found, update the size, and the appropriate pointers */
	    leftOver = remSize;
	    newTP = ptr;
	    oldTP = oldP;
	}
        oldP = ptr; /* set old pointer, auto increment iterator pointer */
    }
    /* Save the located chunk */
    newP = newTP;
    oldP = oldTP;

    if(newP == NULL) { /* no fit found - fail */
        FILE *tp; 
	tp = fopen("malloc_fail.txt", "a+");
        if(totalFree < size) { 
	    /* Lack of memory - log to file */
	    fprintf(tp, " Lack of memory occurred!\n ");
	} else if(totalFree >= size) {
	    /* External Fragmentation Detected - log to file */
	    fprintf(tp, " External Fragmentation occurred!\n ");
	}
        fclose(tp);
        return NULL;
    } 
    void* c = splitting(newP, oldP, size);

    /* Store the measurements in the file and close it */
    clock_gettime(CLOCK_MONOTONIC, &s_end);
    fprintf(fp, "%lld\n", ((long long)s_end.tv_nsec - (long long)s_begin.tv_nsec));
    fclose(fp);

    return c;
}

/* Split larger memory regions into smaller ones, the requested    *
 * space is returned. The leftover space remains on the free list. */
void* splitting(node_t* p, node_t* op, int size) {
    /* Subtract the allocation size from the remaining size */
    int remSize = (p->size + nodeSize) - (size + headSize);
    header_t* temp = (header_t*)(p);
    if(remSize < nodeSize) {
  	size += remSize;
        p = p->next;
    } else {
        node_t storeThis = (*p);
        p = (node_t*)((char*)p + (size + headSize));
        p->size = remSize - nodeSize;
        p->next = storeThis.next;	
    }
    if(op)
        op->next = p;
    else
        head = p;
    temp->size = size;
    temp->magic = MAGICVAL;
    /* header is placed infront of the allocated space */
    return ((char*)temp + headSize);
}

/* Free the memory of the passed pointer */
int psufree(void* ptr) {
    /* Measure the performance of this function */
    fp = fopen("measure_free.txt", "a+"); 
    clock_gettime(CLOCK_MONOTONIC, &s_begin);
    
    /* check for null pointer */
    if(ptr == NULL) return -1;
   
    ptr=(void*)((char*)ptr-headSize);
    /* check if the chunk is allocated */
    if(((header_t*)ptr)->magic != MAGICVAL) return -1;      
   
    node_t* temp = (node_t*)ptr;
    temp->size = (headSize + ((header_t*)ptr)->size) - nodeSize;
    temp->next = head;
    head = temp;
    coalesce();

    /* Store the measurements in the file and close it */
    clock_gettime(CLOCK_MONOTONIC, &s_end);
    fprintf(fp, "%lld\n", ((long long)s_end.tv_nsec - (long long)s_begin.tv_nsec));
    fclose(fp);

    return 0;
}

/* Aid in debugging this mess */
void psumemdump() {
    printf("Free List:\n\n");
    for(node_t* toDump; toDump != NULL; toDump = toDump->next)
        printf("Node: %p\nSize: %d\nNext: %p\n\n", toDump, toDump->size, toDump->next);	
}

/* Search the data structure for memory chunks that can be combined */
void coalesce() {//currentPtr
    for(node_t* ptr = head; ptr != NULL; ptr = ptr->next) {
        /* leadPtr for ptr */        
      	//node_t* nextPtr = (node_t*)((char*)ptr + sizeof(node_t) + ptr->size);
        node_t* nextPtr = (node_t*)((char*)ptr + nodeSize + ptr->size);
	//nextC
        node_t* followPtr = head;
	//prevPtr
	node_t* leadPtr = head; 
	//check
        //for(node_t* leadPtr = head; leadPtr != NULL; ) { 
	while(leadPtr != NULL) {
            if(leadPtr == nextPtr) {
	        /* Free chunk found, combine with surrounding memory */
                followPtr->next = leadPtr->next; 
		
                //ptr->size += (leadPtr->size + sizeof(node_t));  
		ptr->size += (leadPtr->size + nodeSize);             
                //nextPtr = (node_t*)((char*)ptr + sizeof(node_t) + ptr->size);  
		nextPtr = (node_t*)((char*)ptr + nodeSize + ptr->size);
                if(leadPtr != head) {
		    /* Shift the lead pointer, make the common case fast */
		    leadPtr->next = NULL;
                    leadPtr = followPtr->next;
                } else {
		    /* Chunk is located at head, shift it and reset it */
                    head = head->next;
		    leadPtr->next = NULL;         
                    leadPtr = head;
                }
		/* Free chunk located and handled, stop loop */
		//leadPtr->next = NULL;
            } else {
	        /* Update the follower */
                followPtr = leadPtr;
		leadPtr = leadPtr->next;
            }
        }
    }
}



