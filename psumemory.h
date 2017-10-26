#ifndef PSUMEM_H
#define PSUMEM_H

typedef struct __node_t
{
    int size;
    struct __node_t* next;
} node_t;

typedef struct __header_t
{
    int size;
    int magic; 
} header_t;


extern node_t* head;	
int psumeminit(int, int);
void* psumalloc(int);
int psufree(void*);
void* breakUp(node_t*, node_t*, int);
void psumemdump();
void coalesce();
#endif

