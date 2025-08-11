#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/mman.h>
#include <semaphore.h>

#include <stdio.h>
#include <stdlib.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

#define MAX_ARRAY 1024

typedef struct{
    sem_t inDataReady;
    sem_t averageCalculated;
    float array[MAX_ARRAY];
    unsigned arrayLen;
    float average;
} OsInputData;

void* OsCreateMemBlock(const char* fPath, unsigned size){

    int memFd = shm_open(fPath, O_RDWR | O_CREAT, 0600);
    check_error(memFd != -1, "shm_open");
    check_error(ftruncate(memFd, size) != -1, "ftruncate");

    void* addr;
    check_error((addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)) != MAP_FAILED, "mmap");

    close(memFd);

    return addr;
}


int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    OsInputData* data = (OsInputData*)OsCreateMemBlock(argv[1], sizeof(OsInputData));

    check_error(sem_init(&(data->inDataReady), 1, 1) != -1, "sem_init");
    check_error(sem_init(&(data->averageCalculated), 1, 0) != -1, "sem_init");

    scanf("%d", &data->arrayLen);
    for(int i = 0; i < data->arrayLen; i++){
        scanf("%f", &data->array[i]);
    }

    check_error(sem_post(&(data->inDataReady)) != -1, "sem_post");

    check_error(munmap(data, sizeof(OsInputData)) != -1, "munmap");

    exit(EXIT_SUCCESS);
}
