// Program racuna maksimum svih zbirova kolons matrice. Cita fajl, i prva 2 broja
// su broj redova i kolona, a zatim sledi N*M celih brojeva. Pokrece se M niti
// i svaka racuna sumu date kolone i azurira globalni maksimum. Sumu vratiti kroz
// povratnu vrednost niti. Na kraju ispisati sve sume i globalni maksimum. Za
// sinhronizaciju se koristi mutex.
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

#define OsPthreadCheck(pthreadErr, msg)\
    do{\
        int _pthreadErr = pthreadErr;\
        if(_pthreadErr > 0){\
            errno = _pthreadErr;\
            check_error(false, msg);\
        }\
    } while(0)

typedef struct{
    int column;
} OsThreadArg;

typedef struct{
    int sum;
} OsThreadOutput;

int global_max = INT_MIN;
int N, M = 0;
int** matrix = NULL;
pthread_mutex_t global_lock;

void* OsThreadFunc(void* args){

    OsThreadArg* arg = (OsThreadArg*)args;

    int value = 0;
    for(int i = 0; i < N; i++){
        value += matrix[i][arg->column];
    }

    OsPthreadCheck(pthread_mutex_lock(&global_lock), "lock");
    if(global_max < value){
        global_max = value;
    }
    OsPthreadCheck(pthread_mutex_unlock(&global_lock), "unlock");

    OsThreadOutput* output = malloc(sizeof(OsThreadOutput));
    check_error(output != NULL, "malloc");
    output->sum = value;

    return output;
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    FILE* f = fopen(argv[1], "r");
    check_error(f != NULL, "fopen");

    fscanf(f, "%d", &N);
    check_error(N > 0, "Lose N");
    fscanf(f, "%d", &M);
    check_error(M > 0, "Lose M");

    matrix = malloc(N*sizeof(int*));
    check_error(matrix != NULL, "malloc");
    for(int i = 0; i < N; i++){
        matrix[i] = malloc(M*sizeof(int));
        check_error(matrix[i] != NULL, "malloc");
    }

    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            fscanf(f, "%d", &matrix[i][j]);
        }
    }

    fclose(f);

    pthread_t* tids = malloc(M*sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    OsThreadArg* data = malloc(M*sizeof(OsThreadArg));
    check_error(data != NULL, "malloc");

    OsPthreadCheck(pthread_mutex_init(&global_lock, NULL), "init");

    for(int i = 0; i < M; i++){
        data[i].column = i;
        OsPthreadCheck(pthread_create(&tids[i], NULL, OsThreadFunc, &data[i]), "create");
    }

    for(int i = 0; i < M; i++){
        OsThreadOutput* data_output = NULL;
        OsPthreadCheck(pthread_join(tids[i], (void**)&data_output), "join");
        printf("%d ", data_output->sum);
        free(data_output);
    }
    printf("%d\n", global_max);

    OsPthreadCheck(pthread_mutex_destroy(&global_lock), "destroy");

    for(int i = 0; i < N; i++){
        free(matrix[i]);
    }
    free(matrix);
    free(tids);
    free(data);

    exit(EXIT_SUCCESS);
}
