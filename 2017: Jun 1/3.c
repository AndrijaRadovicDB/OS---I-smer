// Program mnozi 2 matrice dim NxM i MxK, potrebno je pokrenuti NxK niti tako da
// svaka nit racuna 1 element matrice, cuvati maks element matrice, korisitit
// mutex za sinhronizaciju i na kraju ispisati novu matricu i maks element
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <pthread.h>
#include <limits.h>

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

#define osCheckThread(pthreadErr, msg)\
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
    int column;
} osThreadFunctionArg;

int N = 0, M = 0, K = 0;
int** matrix1;
int** matrix2;
int** final_matrix;
int global_max = INT_MIN;
pthread_mutex_t global_lock;

void* osThreadFunc(void* args){

    osThreadFunctionArg* arg = (osThreadFunctionArg*)args;

    int vrednost = 0;
    for(int i = 0; i < M; i++){
        vrednost += matrix1[arg->row][i]*matrix2[i][arg->column];
    }

    osCheckThread(pthread_mutex_lock(&global_lock), "lock");
    final_matrix[arg->row][arg->column] = vrednost;
    if(vrednost > global_max){
        global_max = vrednost;
    }
    osCheckThread(pthread_mutex_unlock(&global_lock), "unlock");

    return NULL;
}

int main(int argc, char** argv){

    scanf("%d%d", &N, &M);
    matrix1 = malloc(N*sizeof(int*));
    check_error(matrix1 != NULL, "malloc");
    for(int i = 0; i < N; i++){
        matrix1[i] = malloc(M*sizeof(int));
        check_error(matrix1[i] != NULL, "malloc");
    }
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            scanf("%d", &matrix1[i][j]);
        }
    }

    scanf("%d%d", &M, &K);
    matrix2 = malloc(M*sizeof(int*));
    check_error(matrix2 != NULL, "malloc");
    for(int i = 0; i < M; i++){
        matrix2[i] = malloc(K*sizeof(int));
        check_error(matrix2[i] != NULL, "malloc");
    }
    for(int i = 0; i < M; i++){
        for(int j = 0; j < K; j++){
            scanf("%d", &matrix2[i][j]);
        }
    }

    final_matrix = malloc(N*sizeof(int*));
    check_error(final_matrix != NULL, "malloc");
    for(int i = 0; i < N; i++){
        final_matrix[i] = malloc(K*sizeof(int));
        check_error(final_matrix[i] != NULL, "malloc");
    }

    pthread_t* tids = malloc(N*K*sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    osThreadFunctionArg* args = malloc(N*K*sizeof(osThreadFunctionArg));
    check_error(args != NULL, "malloc");

    osCheckThread(pthread_mutex_init(&global_lock, NULL), "init");

    for(int i = 0; i < N; i++){
        for(int j = 0; j < K; j++){
            args[i*K + j].tid = (i*K + j + 1);
            args[i*K + j].row = i;
            args[i*K + j].column = j;
            osCheckThread(pthread_create(&tids[i*K + j], NULL, osThreadFunc, (void*)&args[i*K + j]), "create");
        }
    }

    for(int i = 0; i < N*K; i++){
        osCheckThread(pthread_join(tids[i], NULL), "join");
    }

    osCheckThread(pthread_mutex_destroy(&global_lock), "destroy");

    for(int i = 0; i < N; i++){
        for(int j = 0; j < K; j++){
            printf("%d ", final_matrix[i][j]);
        }
        printf("\n");
    }
    printf("%d\n", global_max);

    for(int i = 0; i < N; i++){
        free(matrix1[i]);
        free(final_matrix[i]);
    }
    free(matrix1);
    free(final_matrix);
    for(int i = 0; i < M; i++){
        free(matrix2[i]);
    }
    free(matrix2);
    free(tids);
    free(args);

    exit(EXIT_SUCCESS);
}
