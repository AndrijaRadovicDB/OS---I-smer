// Program radi sa objektima deljene memorije i menja sva mala slova u velika
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/mman.h>
#include <semaphore.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

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
    sem_t dataProcessed;
    char str[ARRAY_MAX];
} OsInputData;

void* osGetMemBlock(char* fPath, int* size){

    int memFd = shm_open(fPath, O_RDWR, 0600);
    check_error(memFd != -1, "shm_open");

    struct stat fInfo;
    check_error(fstat(memFd, &fInfo) != -1, "fstat");
    *size = fInfo.st_size;

    void* addr;
    check_error((addr = mmap(NULL, *size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)) != MAP_FAILED, "mmap");

    close(memFd);

    return addr;
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    int size = 0;
    OsInputData* data = (OsInputData*)osGetMemBlock(argv[1], &size);

    check_error(sem_wait(&(data->inDataReady)) != -1, "sem_wait");

    for(int i = 0; data->str[i]; i++){
        if(islower(data->str[i])){
            data->str[i] = toupper(data->str[i]);
        }
    }

    check_error(sem_post(&(data->dataProcessed)) != -1, "sem_post");

    check_error(munmap(data, size) != -1, "munmap");

    exit(EXIT_FAILURE);
}
