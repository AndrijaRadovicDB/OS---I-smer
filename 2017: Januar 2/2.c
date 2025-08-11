// Program racuna maksimum svih zbirova kolona matrice dim N*M, pokrece se M niti
// i u svakoj racunamo sumu kolone, koristi se mutex za sinhronizaciju
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
    int column;
} osThreadFuncArg;

int** matrix;
int m = 0, n = 0;
int max_zbir = INT_MIN;
pthread_mutex_t global_lock;

void* osThreadFunc(void* args){

    osThreadFuncArg* arg = (osThreadFuncArg*) args;

    int value = 0;
    for(int i = 0; i < n; i++){
        value += matrix[i][arg->column];
    }

    osCheckPthread(pthread_mutex_lock(&global_lock), "lock");
    if(value > max_zbir){
        max_zbir = value;
    }
    osCheckPthread(pthread_mutex_unlock(&global_lock), "unlock");

    return NULL;
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    FILE* f = fopen(argv[1], "r");
    check_error(f != NULL, "fopen");

    fscanf(f, "%d%d", &n, &m);

    matrix = malloc(n*sizeof(int*));
    check_error(matrix != NULL, "malloc");
    for(int i = 0; i < n; i++){
        matrix[i] = malloc(m*sizeof(int));
        check_error(matrix[i] != NULL, "malloc");
    }

    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            fscanf(f, "%d", &matrix[i][j]);
        }
    }

    fclose(f);

    pthread_t* tids = malloc(m*sizeof(pthread_t));
    check_error(tids != NULL, " malloc");
    osThreadFuncArg* args = malloc(m*sizeof(osThreadFuncArg));
    check_error(args != NULL, "malloc");

    osCheckPthread(pthread_mutex_init(&global_lock, NULL), "init");

    for(int i = 0; i < m; i++){
        args[i].column = i;
        osCheckPthread(pthread_create(&tids[i], NULL, osThreadFunc, (void*)&args[i]), "create");
    }

    for(int i = 0; i < m; i++){
        osCheckPthread(pthread_join(tids[i], NULL), "join");
    }

    osCheckPthread(pthread_mutex_destroy(&global_lock), "destroy");

    printf("%d\n", max_zbir);

    for(int i = 0; i < n; i++){
        free(matrix[i]);
    }
    free(matrix);
    free(tids);
    free(args);

    exit(EXIT_SUCCESS);
}
