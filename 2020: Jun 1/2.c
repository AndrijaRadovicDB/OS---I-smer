// Program ucitava matricu n*m tipa float, pokrece n niti i svaka vraca sumu 1
// reda. U mainu odrediti minimalnu sumu i ispisati je.
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

#define osPthreadErr(pthreadErr, msg)\
    do{\
        int _pthreadErr = pthreadErr;\
        if(_pthreadErr > 0){\
            errno = _pthreadErr;\
            check_error(false, msg);\
        }\
    } while(0)

typedef struct{
    int row;
    float sum;
} OsThreadArg;

int n, m;
float** matrix;

void* OsThreadFunc(void* args){

    OsThreadArg* arg = (OsThreadArg*)args;

    float value = 0;
    for(int j = 0; j < m; j++){
        value += matrix[arg->row][j];
    }

    OsThreadArg* output = malloc(sizeof(OsThreadArg));
    check_error(output != NULL, "malloc");
    output->sum = value;

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
    OsThreadArg* data = malloc(n*sizeof(pthread_t));
    check_error(data != NULL, "malloc");

    for(int i = 0; i < n; i++){
        data[i].row = i;
        osPthreadErr(pthread_create(&tids[i], NULL, OsThreadFunc, &data[i]), "create");
    }

    float min_sum = INT_MAX;
    for(int i = 0; i < n; i++){
        OsThreadArg* tmp = NULL;
        osPthreadErr(pthread_join(tids[i], (void**)&tmp), "join");
        if(tmp->sum < min_sum){
            min_sum = tmp->sum;
        }
        free(tmp);
    }
    printf("%f\n", min_sum);

    for(int i = 0; i < n; i++){
        free(matrix[i]);
    }
    free(matrix);
    free(tids);
    free(data);

    exit(EXIT_SUCCESS);
}
