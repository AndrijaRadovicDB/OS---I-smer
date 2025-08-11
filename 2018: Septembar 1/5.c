// Program dobija objekt deljene memorije, i ceka na signal posle kog radi
// sledece. Ako je SIGUSR1 - menja znak svim elementima u nizu, SIGUSR2 - duplira
// ih sve. Na kraju povecati semafor
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <signal.h>
#include <sys/mman.h>
#include <semaphore.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

#define ARRAY_MAX (1024)

typedef struct{
    sem_t dataProcessingFinished;
    int array[ARRAY_MAX];
    unsigned arrayLen;
} OsInputData;

bool change_sign = false;
bool double_num = false;

void osHandlerSIGUSR1(int signum){
    change_sign = true;
}

void osHandlerSIGUSR2(int signum){
    double_num = true;
}

void* osGetMemBlock(char* fPath, int* size){

    int memFd = shm_open(fPath, O_RDWR, 0600);
    check_error(memFd != -1, "shm_open");

    struct stat fInfo;
    check_error(fstat(memFd, &fInfo) != -1, "fstat");
    *size = fInfo.st_size;

    void* addr;
    check_error((addr = mmap(NULL, *size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)) != MAP_FAILED, "mman");

    close(memFd);

    return addr;
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    int size = 0;
    OsInputData* data = (OsInputData*)osGetMemBlock(argv[1], &size);

    check_error(signal(SIGUSR1, osHandlerSIGUSR1) != SIG_ERR, "SIGUSR1");
    check_error(signal(SIGUSR2, osHandlerSIGUSR2) != SIG_ERR, "SIGUSR2");
    pause();

    if(change_sign){
        for(int i = 0; i < data->arrayLen; i++){
            data->array[i] = -data->array[i];
        }
        change_sign = false;
    }
    else if(double_num){
        for(int i = 0; i < data->arrayLen; i++){
            data->array[i] = 2*data->array[i];
        }
        double_num = false;
    }

    check_error(sem_post(&(data->dataProcessingFinished)) != -1, "sem_post");

    check_error(munmap(data, size) != -1, "munmap");

    exit(EXIT_SUCCESS);
}
