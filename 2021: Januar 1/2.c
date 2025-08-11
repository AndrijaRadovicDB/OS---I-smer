// Program ucitava n*m matricu tipa float, pokrece n niti, svaka racuna proizvod
// reda i vraca taj proizvod. U mainu izracunati maks proizvod i ispisati ga.
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

#define osPthreadCheck(pthreadErr, msg)\
    do{\
        int _pthreadErr = pthreadErr;\
        if(_pthreadErr > 0){\
            errno = _pthreadErr;\
            check_error(false, msg);\
        }\
    } while(0)

int n, m;
float** matrix;

typedef struct{
    int row;
} OsThreadArg;

typedef struct{
    float value;
} OsThreadOutput;

void* OsThreadFunc(void* args){

    OsThreadArg* arg = (OsThreadArg*)args;

    float value = 1.0;
    for(int j = 0; j < m; j++){
        value *= matrix[arg->row][j];
    }

    OsThreadOutput* output = malloc(sizeof(OsThreadOutput));
    check_error(output != NULL, "malloc");
    output->value = value;

    return output;
}

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    scanf("%d%d", &n, &m);

    matrix = malloc(n*sizeof(float*));
    check_error(matrix != NULL, "malloc");
    for(int i = 0; i < n; i++){
        matrix[i] = malloc(m*sizeof(float));
        check_error(matrix[i] != NULL, "malloc");
    }

    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            scanf("%f", &matrix[i][j]);
        }
    }

    pthread_t* tids = malloc(n*sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    OsThreadArg* data = malloc(n*sizeof(OsThreadArg));
    check_error(data != NULL, "malloc");

    for(int i = 0; i < n; i++){
        data[i].row = i;
        osPthreadCheck(pthread_create(&tids[i], NULL, OsThreadFunc, &data[i]), "create");
    }

    float max_value = INT_MIN;
    for(int i = 0; i < n; i++){
        OsThreadOutput* data_output = NULL;
        osPthreadCheck(pthread_join(tids[i], (void**)&data_output), "join");
        if(data_output->value > max_value){
            max_value = data_output->value;
        }
        free(data_output);
    }
    printf("%f\n", max_value);

    for(int i = 0; i < n; i++){
        free(matrix[i]);
    }
    free(matrix);
    free(tids);
    free(data);

    exit(EXIT_SUCCESS);
}
