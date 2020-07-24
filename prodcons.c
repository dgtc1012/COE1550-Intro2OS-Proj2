/* 
* Skeleton code for prodcons problem
* (C) Mohammad H. Mofrad, 2016
*   
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <linux/prodcons.h>

//mmap pointers to the buffer containing the pancakes
void *BUF_BASE_PTR;
//mmap pointers to the buffer containing the semaphores
void *SEM_BASE_PTR;

int numProducers, numConsumers, bufSize;

void main(int argc, char *argv[])
{
    //read in arguments from user, number of producers, consumers, and size of buffer
    if(argc != 4){
        printf("incorrect arguments: input number of producers, number of consumers, and buf size.");
        exit(1);
    }
    
    numProducers = atoi(argv[1]);
    numConsumers = atoi(argv[2]);
    bufSize = atoi(argv[3]);
    
    //Create buffer that keeps track of the semaphores 
    SEM_BASE_PTR = (void *) mmap(NULL, 4*sizeof(struct cs1550_sem), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);
    if(SEM_BASE_PTR == (void *) -1) 
    {
        fprintf(stderr, "Error mapping memory\n");
        exit(1);
    }

    int *sem_base_ptr = SEM_BASE_PTR;
    int *sem_new_ptr;
    int *sem_curr_ptr = SEM_BASE_PTR;
    int sem_struct_size = sizeof(struct cs1550_sem);
    
    //init full semaphore
    sem_curr_ptr = sem_curr_ptr + sem_struct_size;
    if(sem_curr_ptr > sem_base_ptr + 4*sizeof(struct cs1550_sem)) 
    {
        fprintf(stderr, "INIT FULL: Address out of range\n");
        exit(1);
    }
    else
    {
        sem_new_ptr = sem_curr_ptr - sem_struct_size;
    }
    struct cs1550_sem *full = (struct cs1550_sem *) sem_new_ptr;
    full->value = 0;
    full->start = NULL;
    full->end = NULL;
    
    //init empty semaphore
    sem_curr_ptr = sem_curr_ptr + sem_struct_size;
    if(sem_curr_ptr > sem_base_ptr + 4*sizeof(struct cs1550_sem)) 
    {
        fprintf(stderr, "INIT EMPTY: Address out of range\n");
        exit(1);
    }
    else
    {
        sem_new_ptr = sem_curr_ptr - sem_struct_size;
    }
    struct cs1550_sem *empty = (struct cs1550_sem *) sem_new_ptr;
    empty->value = bufSize;
    empty->start = NULL;
    empty->end = NULL;
    
    //init mutex semaphore
    sem_curr_ptr = sem_curr_ptr + sem_struct_size;
    if(sem_curr_ptr > sem_base_ptr + 4*sizeof(struct cs1550_sem)) 
    {
        fprintf(stderr, "INIT MUTEX: Address out of range\n");
        exit(1);
    }
    else
    {
        sem_new_ptr = sem_curr_ptr - sem_struct_size;
    }
    struct cs1550_sem *mutex = (struct cs1550_sem *) sem_new_ptr;
    mutex->value = 1;
    mutex->start = NULL;
    mutex->end = NULL;
    
    //intialize buffer that will contain pancakes
    //buffer also contains the data for variables needed in both processes
    BUF_BASE_PTR = (void *) mmap(NULL, bufSize+5*sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);
    if(BUF_BASE_PTR == (void *) -1) 
    {
        fprintf(stderr, "Error mapping memory\n");
        exit(1);
    }
    int *buf_curr_ptr = BUF_BASE_PTR;
    
    int *in = buf_curr_ptr;                     //input index in buffer for producer
    int *out = buf_curr_ptr + 1;                //output index in buffer for consumer 
    int *pitem = buf_curr_ptr + 2;              //pancake count
    int *buf_size_ptr = buf_curr_ptr + 3;       //size of buffer
    int *buf_ptr = buf_curr_ptr + 4;            //location in mmap of start of data buffer 
    
    *in = 0;                                    //input index starts at 0
    *out = 0;                                   //output index starts at 0
    *pitem = 0;                                 //pancake count starts at 0
    *buf_size_ptr = bufSize;                    //store size of buffer in mmap
    *buf_ptr = buf_curr_ptr + 5*sizeof(int);    //store address for start of data buffer
    
    int i;
    int pid; // process id returned by fork
    
    //Fork processes to become producers
    for(i = 0; i<numProducers; i++){
        pid = fork();
        //if process ID is 0, then this process is the child process and you should start doing work
        if(pid == 0){
            //identifier for each process is an alphabet character
            //determine the ascii value for the character representation of each process
            int chef = i + 65;
            char chef_name = chef;
            while(1){
                cs1550_down(empty);
                cs1550_down(mutex);
                buf_ptr[*in] = *pitem;

                printf("Chef %c Produced: Pancake%d\n", chef_name, *pitem);

                *in = ((*in)+1) % *buf_size_ptr;
                (*pitem)++;
                cs1550_up(mutex);
                cs1550_up(full);
            }
        }
    }
    
    //fork processes to become consumers
    for(i = 0; i<numConsumers; i++){
        pid = fork();
        //if process ID is 0, then this process is the child process and you should start doing work
        if(pid == 0){
            int citem;
            //identifier for each process is an alphabet character
            //determine the ascii value for the character representation of each process
            int consumer = i + 65;
            char consumer_name = consumer;
            while(1){
                cs1550_down(full);
                cs1550_down(mutex);
                citem = buf_ptr[*out];
                
                printf("Customer %c Consumed: Pancake%d\n", consumer_name, citem);
                
                *out = ((*out)+1) % *buf_size_ptr;
                cs1550_up(mutex);
                cs1550_up(empty);
            }
        }
    }
    //wait for ctrl+c interupt, kill all child processes and terminate.
    int status;
    wait(&status);
    return;
}

void cs1550_down(struct cs1550_sem *sem) 
{
syscall(__NR_sys_cs1550_down, sem);
}

void cs1550_up(struct cs1550_sem *sem) 
{
syscall(__NR_sys_cs1550_up, sem);
}
