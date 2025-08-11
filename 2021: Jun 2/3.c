// Program radi sa deljenom memorijom. On treba sve brojeve koji u svom binarnom
// zapisu sadrze 4 1 da prebaci u izlaznu memoriju. Semafor ulazne memorije
// umanjiti, izlazne uvecati
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
    int array[ARRAY_MAX];
    unsigned arrayLen;
} OsData;

void* OsGetMemBlock(char* fPath, int* size){

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

int num_of_1(int num){

    int num_1 = 0;
    unsigned mask = 1;

    while(mask){

        if(num & mask){
            num_1++;
        }
        if(num_1 >= 4){
            return 1;
        }

        mask <<= 1;
    }

    return 0;
}

int main(int argc, char** argv){

    check_error(argc == 3, "Argumenti");

    int input_size = 0;
    OsData* input_data = (OsData*)OsGetMemBlock(argv[1], &input_size);
    int output_size = 0;
    OsData* output_data = (OsData*)OsGetMemBlock(argv[2], &output_size);

    check_error(sem_wait(&(input_data->inDataReady)) != -1, "sem_wait");

    int j = 0;
    for(int i = 0; i < input_data->arrayLen; i++){
        if(num_of_1(input_data->array[i])){
            output_data->array[j] = input_data->array[i];
            j++;
        }
    }
    output_data->arrayLen = j;

    check_error(sem_post(&(output_data->inDataReady)) != -1, "sem_post");

    check_error(munmap(input_data, input_size) != -1, "munmap");
    check_error(munmap(output_data, output_size) != -1, "munmap");

    exit(EXIT_SUCCESS);
}
