// Program ucitava broj cvorova, matricu povezanosti grafa dim V*V,pokrece
// se V niti i u svakoj se izracunava stepen od cvora, ispisati indeks cvora sa
// najvecim ulaznim stepenom
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
#include <limits.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

#define osCheckPthread(pthreadErr, msg)\
    do{\
        int _pthreadErr = pthreadErr;\
        if(_pthreadErr > 0){\
            errno = _pthreadErr;\
            check_error(false, msg);\
        }\
    } while(0)

typedef struct{
    int index;
} osThreadArg;

int** M;
int V = 0;
int global_max = INT_MIN;
int global_max_index = 0;
pthread_mutex_t global_lock;

void* osThreadFunc(void* args){

    osThreadArg* arg = (osThreadArg*)args;

    int value = 0;
    for(int i = 0; i < V; i++){
        value += M[i][arg->index];
    }

    osCheckPthread(pthread_mutex_lock(&global_lock), "lock");
    if(value > global_max){
        global_max = value;
        global_max_index = arg->index;
    }
    osCheckPthread(pthread_mutex_unlock(&global_lock), "unlock");

    return NULL;
}

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    scanf("%d", &V);
    M = malloc(V*sizeof(int*));
    check_error(M != NULL, "malloc");
    for(int i = 0; i < V; i++){
        M[i] = malloc(V*sizeof(int));
        check_error(M[i] != NULL, "malloc");
    }

    for(int i = 0; i < V; i++){
        for(int j = 0; j < V; j++){
            scanf("%d", &M[i][j]);
        }
    }

    pthread_t* tids = malloc(V*sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    osThreadArg* args = malloc(V*sizeof(osThreadArg));
    check_error(args != NULL, "malloc");

    osCheckPthread(pthread_mutex_init(&global_lock, NULL), "init");

    for(int i = 0; i < V; i++){
        args[i].index = i;
        osCheckPthread(pthread_create(&tids[i], NULL, osThreadFunc, &args[i]), "create");
    }

    for(int i = 0; i < V; i++){
        osCheckPthread(pthread_join(tids[i], NULL), "join");
    }

    osCheckPthread(pthread_mutex_destroy(&global_lock), "destroy");

    printf("%d\n", global_max_index);

    for(int i = 0; i < V; i++){
        free(M[i]);
    }
    free(M);
    free(tids);
    free(args);

    exit(EXIT_SUCCESS);
}
