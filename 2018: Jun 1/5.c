// Program radi sa objektima deljene memorije. Program pronalazi 2 najblize tacke
// i na kraju ispisuje najkrace rastojanje koje je izracunao
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/mman.h>
#include <semaphore.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

#define MAX_SIZE (1024)

typedef struct{
    sem_t ready;
    sem_t done;
    double points[MAX_SIZE];
    unsigned nPoints;
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
    double min_dist = INT_MAX;

    check_error(sem_wait(&(data->ready)) != -1, "sem_wait");
    for(int i = 0; i < data->nPoints; i += 2){
        for(int j = 0; j < data->nPoints; j += 2){
            if(i != j){
                double distance = pow(data->points[i] - data->points[j], 2) + pow(data->points[i + 1] - data->points[j + 1], 2);
                if(distance < min_dist){
                    min_dist = distance;
                }
            }
        }

    }
    check_error(sem_post(&(data->done)) != -1, "sem_post");

    printf("%lf\n", sqrt(min_dist));

    exit(EXIT_SUCCESS);
}
