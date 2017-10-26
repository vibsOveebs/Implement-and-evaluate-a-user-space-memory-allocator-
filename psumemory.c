#include <assert.h>
#include "psumemory.h"
#include <sys/mman.h>
#include <unistd.h> 
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
int magicVal =  123456;
node_t* head = NULL;
int fit;
int psumeminit(int algo, int SIZE)
{   fit = algo;      
    head = mmap(NULL, SIZE, PROT_READ | PROT_WRITE,MAP_ANON | MAP_PRIVATE, -1, 0);
    if(!head)
        return -1;
    head->size = SIZE - sizeof(node_t);
    head->next = NULL;
    return 0;
}


void* psumalloc(int SIZE)
{   node_t* p = head;
    node_t* newP = NULL, *oldP = NULL;
    const int deltaSize = sizeof(node_t) - sizeof(header_t);  
    if(fit == 0)  
    {   int minLeft = INT_MAX;
        node_t* newBFP = NULL, *oldBFP = NULL;
        while(p != NULL)
        {
            int currentSize = p->size;
            int leftSize = (currentSize-SIZE) + deltaSize;
            if((leftSize >=0) && leftSize <= minLeft)
            {   minLeft = leftSize;
                newBFP = p;
                oldBFP = oldP;
            }
            oldP = p;
            p = p->next;
        }
        newP = newBFP;
        oldP = oldBFP;
    }
    else if(fit == 1)
    {   int maxLeft = 0;
        node_t* newMaxP = NULL, *oldMaxP = NULL;
        while(p != NULL)
        {   int currentSize = p->size;
            int leftSize = (currentSize-SIZE) + deltaSize;
            if(leftSize > maxLeft)
            {   maxLeft = leftSize;
                newMaxP = p;
                oldMaxP = oldP;
            }
            oldP = p;
            p = p->next;
        }
        newP = newMaxP;
        oldP = oldMaxP;
    }  
    if(!newP){
        return NULL; 
    }
    void* c = breakUp(newP, oldP, SIZE);
    return c;
}


void* breakUp(node_t* p, node_t* oldP, int SIZE)
{
    int allocate = SIZE + sizeof(header_t);
    int deltaSize = (p->size + sizeof(node_t));
    int leftSize = deltaSize - allocate;
    header_t* temp = (header_t*)(p);
    if(leftSize > sizeof(node_t))
    {  	node_t storeThis = (*p);
        p = (node_t*)((char*)p + allocate);
        p->size = leftSize - sizeof(node_t);
        p->next = storeThis.next;
    }
    else
    {	SIZE += leftSize;
        p = p->next;
    }
    if(oldP)
        oldP->next = p;
    else
        head = p;
    temp->size = SIZE;
    temp->magic = magicVal;
    return ((char*)temp + sizeof(header_t));
}

int psufree(void* p)
{
    if(!p)
        return -1;    
    p=(void*)((char*)p-sizeof(header_t));
    if(((header_t*)p)->magic != magicVal)
        return -1;    
    const int totalSize = sizeof(header_t) + ((header_t*)p)->size;
    const int nSize = totalSize - sizeof(node_t);
    node_t* nP = (node_t*)p;
    nP->size = nSize;
    nP->next = head;
    head = nP;
    coalesce();
    return 0;
}

void psumemdump()
{
    printf("Free List:\n\n");
    node_t* toDump = head;
    while(toDump != NULL)
    {
        printf("Node: %p\nSize: %d\nNext: %p\n\n", toDump, toDump->size, toDump->next);	
       	toDump = toDump->next;
    }
}

void coalesce()
{   node_t* thisP = head;
    while(thisP != NULL)
    {  node_t* tester = head, *oldP = head;
       node_t* nextC = (node_t*)((char*)thisP + sizeof(node_t) + thisP->size);
        while(tester != NULL)
        {
            if(tester == nextC)
            {
                oldP->next = tester->next;
                int testerSize = tester->size + sizeof(node_t);
                tester->size += testerSize;
                nextC = (node_t*)((char*)thisP + sizeof(node_t) + thisP->size);
                if(tester == head)
                {   head = head->next;
                    tester->next = NULL;
                    tester = head;
                }
                else
                {   tester->next = NULL;
                    tester = oldP->next;
                }
            }
            else
            {
                oldP = tester;
                tester = tester->next;
            }
        }
        thisP = thisP->next;
    }
}




