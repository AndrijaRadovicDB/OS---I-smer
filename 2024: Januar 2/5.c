// Program radi sa objektima deljene memorije. Program ceka na semaforu pa racuna
// aritmeticku sredinu brojeva niza i upisuje vrednopst u average, i onda
// postavlja semafor. Na kraju na stdout se ispisuje aritmeticka sredina.
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

    float sum = 0;
    for(int i = 0; i < data->arrayLen; i++){
        sum += data->array[i];
    }
    float average = sum*1.0/data->arrayLen;
    data->average = average;
    printf("%g\n", average);

    check_error(sem_post(&(data->averageCalculated)) != -1, "sem_post");

    check_error(munmap(data, size) != -1, "munmap");

    exit(EXIT_SUCCESS);
}
