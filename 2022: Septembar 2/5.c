// Program radi sa objektima deljene memorije. Treba upisati u naziv dana i na
// stdout dan koji je predstavljen brojem sekundi.
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

#define NAME_MAX 256

typedef struct{
    sem_t inDataReady;
    sem_t operationDone;
    char weekDay[NAME_MAX];
    int timeInSeconds;
} OsInputData;

void* OsGetMemBlock(const char* fPath, unsigned* size){

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

    unsigned size = 0;
    OsInputData* data = (OsInputData*)OsGetMemBlock(argv[1], &size);

    check_error(sem_wait(&(data->inDataReady)) != -1, "sem_wait");

    time_t new_time = data->timeInSeconds;
    struct tm* broken_time = localtime(&new_time);
    check_error(broken_time != NULL, "localtime");

    if(broken_time->tm_wday == 0){
        strcpy(data->weekDay, "nedelja");
    }
    else if(broken_time->tm_wday == 1){
        strcpy(data->weekDay, "ponedeljak");
    }
    else if(broken_time->tm_wday == 2){
        strcpy(data->weekDay, "utorak");
    }
    else if(broken_time->tm_wday == 3){
        strcpy(data->weekDay, "sreda");
    }
    else if(broken_time->tm_wday == 4){
        strcpy(data->weekDay, "cetvrtak");
    }
    else if(broken_time->tm_wday == 5){
        strcpy(data->weekDay, "petak");
    }
    else if(broken_time->tm_wday == 6){
        strcpy(data->weekDay, "subota");
    }

    printf("%s\n", data->weekDay);

    check_error(sem_post(&(data->operationDone)) != -1, "sem_post");

    check_error(munmap(data, size) != -1, "munmap");

    exit(EXIT_SUCCESS);
}
