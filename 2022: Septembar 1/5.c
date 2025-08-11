// Program radi sa objektima deljene memorije. Treba da ceka na semaforu, a zatim
// da odredi velicinu simbolickog linka i da je upise u linkSize, a onda da je
// ispisuje na standardni izlaz. U slucaju da citanje informacija o simbolickom
// linku ne uspe, linkSize postaviti na -1 i to ipisati na standardni izlaz.
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/mman.h>
#include <semaphore.h>

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
    char linkName[NAME_MAX];
    int linkSize;
} OsInputData;

void* OsGetMemBlock(const char* fPath, unsigned* size){

    int memFd = shm_open(fPath, O_RDWR, 0600);
    check_error(memFd != -1, "memFd");

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

    struct stat lInfo;
    int ret_val = lstat(data->linkName, &lInfo);
    if(ret_val != -1 && S_ISLNK(lInfo.st_mode)){
        data->linkSize = lInfo.st_size;
        printf("%ld\n", lInfo.st_size);
    }
    else{
        data->linkSize = -1;
        printf("-1\n");
    }

    check_error(sem_post(&(data->operationDone)) != -1, "sem_post");

    check_error(munmap(data, size) != -1, "munmap");

    exit(EXIT_SUCCESS);
}
