// Program prima objekte deljenje memorije, na izlaz ispisuje sve brojeve ciji binarni zapis ima bar 4 jedinice u sebi, pre obrade se ceka semafor
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

#define ARRAY_MAX 1024

typedef struct{
    sem_t inDataReady;
    int array[ARRAY_MAX];
    unsigned arrayLen;
} OsInputData;

void* osGetMemoryBlock(char* fPath, int* size){

    int memFd = shm_open(fPath, O_RDWR, 0600);
    check_error(memFd != -1, "shm_open");

    struct stat fInfo;
    check_error(fstat(memFd, &fInfo) != -1, "fstat");
    *size = fInfo.st_size;

    void* addr;

    check_error((addr = mmap(0, *size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)) != MAP_FAILED, "mmap");

    close(memFd);

    return addr;
}

int broj_jedinica(int vrednost){

    int br_jedinica = 0;

    while(vrednost){
        if(vrednost & 1){
            br_jedinica++;
        }
        vrednost >>= 1;
    }

    return br_jedinica;
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    int size = 0;
    OsInputData* data = (OsInputData*)osGetMemoryBlock(argv[1], &size);

    check_error(sem_wait(&(data->inDataReady)) != -1, "sem_wait");

    for(int i = 0; i < data->arrayLen; i++){
        if(broj_jedinica(data->array[i]) >= 4){
            printf("%d ", data->array[i]);
        }
    }
    printf("\n");

    check_error(munmap(data, size) != -1, "munmap");

    exit(EXIT_SUCCESS);
}
