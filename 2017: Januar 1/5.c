// Program prima pitanje do objekata deljenje memorije, trazi medijanu niza, i
// pre svake obrade treba sacekati na semafor inDataReady
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <semaphore.h>
#include <sys/mman.h>

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

void* osGetMemoryBlock(char* fPath, int* size){

    int memFd = shm_open(fPath, O_RDWR, 0600);
    check_error(memFd != -1, "shm_open");

    struct stat fInfo;
    check_error(fstat(memFd, &fInfo) != -1, "fstat");
    *size = fInfo.st_size;

    // Mapiranje
    void* addr;
    check_error((addr = mmap(0, *size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)) != MAP_FAILED, "mmap");

    close(memFd);

    return addr;
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    int size = 0;
    OsInputData* data = osGetMemoryBlock(argv[1], &size);

    check_error(sem_wait(&(data->inDataReady)) != -1, "sem_wait");

    // Sortira niz
    for(int i = 0; i < data->arrayLen; i++){
        for(int j = i + 1; j < data->arrayLen; j++){
            if(data->array[i] > data->array[j]){
                float tmp = data->array[i];
                data->array[i] = data->array[j];
                data->array[j] = tmp;
            }
        }
    }

    // Ispisuje sredisnji element - medijanu
    printf("%f\n", data->array[data->arrayLen/2]);

    // Odmapiranje
    check_error(munmap(data, size) != -1, "munmap");

    exit(EXIT_SUCCESS);
}
