// Program dobija objekt deljene memorije i broj kojim sifruje polje s ciklicnim
// dodavanjem broja
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

#define MAX_STR_LEN 256

typedef struct{
    sem_t inDataReady;
    sem_t outDataReady;
    char s[MAX_STR_LEN];
} OsInputData;

void* osGetMemoryBlock(char* fPath, int* size){

    int memFds = shm_open(fPath, O_RDWR, 0600);
    check_error(memFds != -1, "shm_open");

    struct stat fInfo;
    check_error(fstat(memFds, &fInfo) != -1, "fstat");
    *size = fInfo.st_size;

    void* addr;

    check_error((addr = mmap(0, *size, PROT_READ | PROT_WRITE, MAP_SHARED, memFds, 0)) != MAP_FAILED, "mmap");

    close(memFds);

    return addr;

}

int main(int argc, char** argv){

    check_error(argc == 3, "Argumenti");

    int size = 0;
    OsInputData* data = osGetMemoryBlock(argv[1], &size);

    int sifra = atoi(argv[2]);

    check_error(sem_wait(&(data->inDataReady)) != -1, "sem_wait");

    for(int i = 0; data->s[i]; i++){
        data->s[i] = (data->s[i] + sifra) % 128; // % 128 jer toliko char drzi
    }

    // Postavljamo semafor
    check_error(sem_post(&(data->outDataReady)) != -1, "sem_post");

    check_error(munmap(data, size) != -1, "munmap");

    exit(EXIT_SUCCESS);
}
