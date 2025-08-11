// Program pomocu niti racuna histrogram slova koja se nalaze u fajlu, 1 nit
// broji pojavljivanje 1 slova, ne razlikuju mala i velika, sve sto nisu slova
// ignorisu. Na kraju ispisuje histrogram i slovo sa najvise pojavljivanja.
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

#define osPthreadCheck(pthreadErr, msg)\
    do{\
        int _pthreadErr = pthreadErr;\
        if(_pthreadErr > 0){\
            errno = _pthreadErr;\
            check_error(false, msg);\
        }\
    } while(0)

#define NUM_OF_THREADS (26)

typedef struct{
    char* fPath;
    char c;
    int position;
} osThreadArg;

char char_max = 0;
int max_value = 0;
int histogram[26] = {0};
pthread_mutex_t global_lock;

void* osThreadFunc(void* args){

    osThreadArg* arg = (osThreadArg*)args;
    int value = 0;

    FILE* f = fopen(arg->fPath, "r");
    check_error(f != NULL, "fopen");

    char c;
    while((c = fgetc(f)) != EOF){
        if(isalpha(c)){
            if(arg->c == tolower(c)){
                value++;
            }
        }
    }

    fclose(f);

    osPthreadCheck(pthread_mutex_lock(&global_lock), "lock");
    if(value > max_value){
        max_value = value;
        char_max = arg->c;
    }
    histogram[arg->position] = value;
    osPthreadCheck(pthread_mutex_unlock(&global_lock), "unlock");

    return NULL;
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    int numThr = NUM_OF_THREADS;

    pthread_t* tids = malloc(numThr*sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    osThreadArg* data = malloc(numThr*sizeof(osThreadArg));
    check_error(data != NULL, "malloc");

    osPthreadCheck(pthread_mutex_init(&global_lock, NULL), "init");

    for(int i = 0; i < numThr; i++){
        data[i].fPath = argv[1];
        data[i].c = 'a' + i;
        data[i].position = i;
        osPthreadCheck(pthread_create(&tids[i], NULL, osThreadFunc, &data[i]), "create");
    }

    for(int i = 0; i < numThr; i++){
        osPthreadCheck(pthread_join(tids[i], NULL), "join");
    }

    osPthreadCheck(pthread_mutex_destroy(&global_lock), "destroy");

    for(int i = 0; i < numThr; i++){
        printf("%d ", histogram[i]);
    }
    printf("\n%c\n", char_max);

    free(tids);
    free(data);

    exit(EXIT_SUCCESS);
}
