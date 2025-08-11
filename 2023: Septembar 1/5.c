// Program radi sa objektima deljene memorije. Ceka semafor, pa filtrira array
// tako da samo ostanu cifre u result i broj cifara upisuje u resultSize. Posle
// postavlja semafor i na stdout pise broj cifata, ako ih nema stavlja svuda -1.
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

#define MAX_SIZE 1024

typedef struct{
    sem_t dataReady;
    sem_t operationDone;
    char array[MAX_SIZE];
    int resultSize;
    char result[MAX_SIZE];
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

    check_error(sem_wait(&(data->dataReady)) != -1, "sem_wait");

    int new_size = 0;
    for(int i = 0; data->array[i]; i++){
        if(isdigit(data->array[i])){
            data->result[new_size++] = data->array[i];
        }
    }
    data->result[new_size] = '\0';
    if(new_size){
        printf("%d\n", new_size);
        data->resultSize = new_size;
    }
    else{
        printf("-1\n");
        data->resultSize = -1;
    }

    check_error(sem_post(&(data->operationDone)) != -1, "sem_post");

    check_error(munmap(data, size) != -1, "munmap");

    exit(EXIT_SUCCESS);
}
