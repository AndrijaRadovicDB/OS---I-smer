// Program prima ime objekta deljene memorije i ispisuje devijaciju niza
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/mman.h>
#include <semaphore.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

void* osGetMemoryBlock(char* fPath, int* size){

    int memFd = shm_open(fPath, O_RDWR, 0600);
    check_error(memFd != -1, "shm_open");

    struct stat fInfo;
    check_error(fstat(memFd, &fInfo) != -1, "fstat");
    *size = fInfo.st_size;

    void* addr;

    check_error((addr = mmap(0, *size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)) != MAP_FAILED, "mmap");

    return addr;
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    int size = 0;
    OsInputData* data = osGetMemoryBlock(argv[1], &size);

    check_error(sem_wait(&(data->inDataReady)) != -1, "sem_wait");

    double ni = 0;
    for(int i = 0; i < data->arrayLen; i++){
        ni += data->array[i];
    }
    ni /= data->arrayLen*1.0;

    double devijacija = 0;
    for(int i = 0; i < data->arrayLen; i++){
        devijacija += pow(data->array[i] - ni, 2);
    }
    devijacija /= data->arrayLen*1.0;
    devijacija = sqrt(devijacija);

    printf("%lf\n", devijacija);

    check_error(munmap(data, size) != -1, "munmap");

    exit(EXIT_SUCCESS);
}
