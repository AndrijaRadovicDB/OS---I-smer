// Program sa vise niti izracunava minimum matrice, dobijamo fajl u kom je matrica
// prva dva broja su broj redova i kolona matrice, a niti treba da bude koliko i
// redova, koristi se mutex za sinhronizaciju
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <sys/wait.h>
#include <pthread.h>

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

#define osCheckPthread(pthreadErr, msg)\
    do{\
        int _pthreadErr = pthreadErr;\
        if(_pthreadErr > 0){\
            errno = _pthreadErr;\
            check_error(false, msg);\
        }\
    } while(0)

typedef struct{
    int tid;
    int row;
} osThreadFuncArgumentType;

double global_min = 0;
pthread_mutex_t global_lock;

int N = 0;
int M = 0;
double** matrix;

void* osThreadFunc(void* arg){

    osThreadFuncArgumentType* args = (osThreadFuncArgumentType*)arg;

    double min = matrix[args->row][0];
    int i = 0;
    for(i = 1; i < M; i++){
        if(matrix[args->row][i] < min){
            min = matrix[args->row][i];
        }
    }

    osCheckPthread(pthread_mutex_lock(&global_lock), "lock");
    if(min < global_min){
        global_min = min;
    }
    osCheckPthread(pthread_mutex_unlock(&global_lock), "unlock");
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    FILE* f = fopen(argv[1], "r");
    check_error(f != NULL, "fopen");

    fscanf(f, "%d%d", &N, &M);
    int i = 0, j = 0;
    matrix = malloc(N*sizeof(double*));
    check_error(matrix != NULL, "malloc");
    for(i = 0; i < N; i++){
        matrix[i] = malloc(M*sizeof(double));
        check_error(matrix[i] != NULL, "malloc");
    }

    for(i = 0; i < N; i++){
        for(j = 0; j < M; j++){
            fscanf(f, "%lf", &matrix[i][j]);
        }
    }

    fclose(f);

    pthread_t* tids = malloc(N*sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    osThreadFuncArgumentType* args = malloc(N*sizeof(osThreadFuncArgumentType));
    check_error(args != NULL, "malloc");

    osCheckPthread(pthread_mutex_init(&global_lock, NULL), "init");

    for(i = 0; i < N; i++){
        args[i].tid = (i + 1);
        args[i].row = i;
        osCheckPthread(pthread_create(&tids[i], NULL, osThreadFunc, (void*)&args[i]), "create");
    }

    for(i = 0; i < N; i++){
        osCheckPthread(pthread_join(tids[i], NULL), "join");
    }

    osCheckPthread(pthread_mutex_destroy(&global_lock), "destroy");

    printf("%lf\n", global_min);

    for(i = 0; i < N; i++){
        free(matrix[i]);
    }
    free(matrix);
    free(tids);
    free(args);

    exit(EXIT_SUCCESS);
}
