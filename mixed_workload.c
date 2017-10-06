#include <stdio.h>
#include "psumemory.h"

#define sizeOfRegion 209715200
#define NUM 100
#define algo 0 //Change to 1 for Worst Fit

typedef struct _test{
    int a;
    int b;
    int c;
    char one;
    char two;
    char three;
} test;

void write_test(test* ptr){
    if (ptr == NULL){
        return;
    }
    ptr->a = 1;
    ptr->b = 2;
    ptr->c = 3;
    ptr->one = 'A';
    ptr->two = 'B';
    ptr->three = 'C';
}

void read_test(test* ptr, FILE* f){
    if (ptr == NULL){
        return;
    }
    fprintf(f, "This struct contains: %d, %d, %d, %c, %c and %c. \n", ptr->a, ptr->b, ptr->c, ptr->one, ptr->two, ptr->three);
}

int main(){
    FILE* f = fopen ("test_output_correctness_1.txt", "w");
    int size;
    int a = psumeminit(algo, sizeOfRegion);
    if (a == -1){
        fprintf(f, "Initialization failed!\n");
    }

    void* pointer_array[100*NUM];
    for (int i = 0; i < 100*NUM; i++) {
        pointer_array[i] = NULL;
    }

    int total = 0;
    for (int i = 0; i < 100; i++){
        for (int j = 0; j < NUM; ++j)
        {
            if (j % 2 != 0){
                size = 64;
            }
            else{
                size = 64 * 1024;
            }
            test* a = (test*)psumalloc(size);
            write_test(a);
            read_test(a, f);
            if (a == NULL){
                fprintf(f, "No.%d has no extra space for allocation in memory!\n", j);
            }
            else{
                total ++;
                int current = 0;
                while (pointer_array[current] != NULL){
                    current ++;
                }
                pointer_array[current] = a;
                fprintf(f, "NO.%d chunk has been allocated, the size is %d bytes\n", j, size);
            }
        }

        int half = NUM / 2;
        int k = 0;
        fprintf(f, "there are %d need to be freed!\n", half);
        while (half != 0 && k < 100*NUM){
            if (pointer_array[k] == NULL){
                k++;
            }
            else{
                int a = psufree(pointer_array[k]);
                if (a == 0){
                    fprintf(f, "No.%d chunk has been freed. \n", k);
                    pointer_array[k] = NULL;
                    half --;
                    total --;
                }
                else{
                    fprintf(f, "Can not free No.%d chunk. \n", k);
                }
                k ++;
            }
        }
    }

    fprintf(f, "Here is final cleaning for %d chunks. \n", total);
    for (int i = 0; i < 100*NUM; i++){
        if (pointer_array[i] != NULL){
            fprintf(f, "address is %p\n", pointer_array[i]);
            int a = psufree(pointer_array[i]);
            if (a == 0){
                fprintf(f, "No.%d chunk has been freed. \n", i);
                pointer_array[i] = NULL;
                total --;
            }
            else{
                fprintf(f, "Can not free No.%d chunk. \n", i);
            }
        }
    }
    fclose(f);
    return 0;
}
