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
    float array[ARRAY_MAX];
    unsigned arrayLen;
} OsInputData;

void* CreateMemBlock(char* fPath, int size){

    int memFd = shm_open(fPath, O_RDWR | O_CREAT, 0600);
    check_error(memFd != -1, "open");
    // Postavlja fajl na datu velicinu, ako smanjuje gube se podaci, ako
    // povecava samo se fajl prosiruje bajtovima
    check_error(ftruncate(memFd, size) != -1, "ftruncate");

    void* addr;
    check_error((addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)) != MAP_FAILED, "mmap");

    close(memFd);

    return addr;
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    OsInputData* data = CreateMemBlock(argv[1], sizeof(OsInputData));

    check_error(sem_init(&(data->inDataReady), 1, 1) != -1, "sem_init");

    scanf("%u", &data->arrayLen);
    for(int i = 0; i < data->arrayLen; i++){
        scanf("%f", &data->array[i]);
    }

    check_error(sem_post(&(data->inDataReady)) != -1, "sem_post");

    check_error(munmap(data, sizeof(OsInputData)) != -1, "munmap");

    exit(EXIT_SUCCESS);
}
