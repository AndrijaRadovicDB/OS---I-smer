// Program za testiranje 5.c
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>

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
    float array[ARRAY_MAX];
    unsigned arrayLen;
} OsInputData;

void* osCreateMemoryBlock(char* fpath, unsigned size){

    int memFd = shm_open(fpath, O_RDWR | O_CREAT, 0600);
    check_error(memFd != -1, "shm_open");

    check_error(ftruncate(memFd, size) != -1, "ftruncate");

    void* addr;
    check_error((addr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)) != MAP_FAILED, "mmap");

    close(memFd);

    return addr;
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    // Proces mora da kreira blok deljenje memorije koji koristi nas zadatak
    OsInputData* data = osCreateMemoryBlock(argv[1], sizeof(OsInputData));

    // Proces koji puni deljenu memoriju MORA da inicijalizuje semafor
    // Proces koji koristi semafor, tj nas zadatak, NE SME da inicijalizuje memoriju
    // Visestruko incijalizovanje semafore dovodi do nedefinisanog ponasanja
    check_error(sem_init(&data->inDataReady, 1, 0) != -1, "sem_init");

    // Seme generatora brojeva
    srand(time(NULL));
    // Slucajan broj incijalizuje broj elemenata
    data->arrayLen = rand() % ARRAY_MAX;

    for(int i = 0; i < data->arrayLen; i++){
        data->array[i] = (float)(rand()/(float)RAND_MAX);
    }

    // Za debugovanje ovde mozemo naci medijanu pa porediti sa nasim programom

    // Postavljanjem na semaforu obavestava se nas zadatak da su podaci spremin
    check_error(sem_post(&data->inDataReady) != -1, "sem_post");

    check_error(munmap(data, sizeof(OsInputData)) != -1, "munmap");

    exit(EXIT_SUCCESS);
}
