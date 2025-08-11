// Program radi sa deljenom memorijom. Kopira sve brojeve iz ulazne memorije koji
// u svom binarnom zapisu imaju bar 4 1. U ulaznoj memoriji umanjiti semafor, a u
// izlaznoj uvecati. Postaviti ispravan broj elemenata arrayLen u izlaznoj mem.
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

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

#define ARRAY_MAX 1024

typedef struct{
    sem_t inDataReady;
    int array[ARRAY_MAX];
    unsigned arrayLen;
} OsData;

int num_of_1(int x){

    int num = 0;

    while(x){
        if(x & 1){
            num++;
            if(num >= 4){
                return true;
            }
        }
        x >>= 1;
    }

    return false;

}

void* osGetMemBlock(char* fPath, int* size){

    int memFd = shm_open(fPath, O_RDWR, 0600);
    check_error(memFd != -1, "shm_open");

    struct stat fInfo;
    check_error(fstat(memFd, &fInfo) != -1, "fstat");
    *size = fInfo.st_size;

    void* addr;
    check_error((addr = mmap(NULL, *size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)) != MAP_FAILED, "mmap");

    return addr;
}

int main(int argc, char** argv){

    check_error(argc == 3, "Argumenti");

    int input_size = 0, output_size = 0;
    OsData* input = (OsData*)osGetMemBlock(argv[1], &input_size);
    OsData* output = (OsData*)osGetMemBlock(argv[2], &output_size);

    check_error(sem_wait(&(input->inDataReady)) != -1, "sem_wait");

    int size = 0;
    for(int i = 0; i < input->arrayLen; i++){
        if(num_of_1(input->array[i])){
            output->array[size] = input->array[i];
            size++;
        }
    }
    output->arrayLen = size;

    check_error(sem_post(&(output->inDataReady)) != -1, "sem_post");

    check_error(munmap(input, input_size) != -1, "munmap");
    check_error(munmap(output, output_size) != -1, "munmap");

    exit(EXIT_SUCCESS);
}
