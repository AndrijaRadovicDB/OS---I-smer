// Program ucitava tacke do kraja ulaza, pokrenuti onoliko niti koliko je tacaka,
// svaka nit racuna rastojanje svoje tacke od svih ostalih, na kraju ispisati
// minimalno rastojanje izmedju bilo koje 2 tacke
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
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

#define MAX_SIZE (256)

typedef struct{
    double x;
    double y;
    int row;
} OsThreadArg;

int numOfStr = 0;
double min_dist = LONG_MAX;
double** points;
pthread_mutex_t global_lock;

void* osThreadFunc(void* args){

    OsThreadArg* arg = (OsThreadArg*)args;

    double value = LONG_MAX;
    for(int i = 0; i < numOfStr; i++){
        if(i != arg->row){
            double dist = pow(points[i][0] - arg->x, 2) + pow(points[i][1] - arg->y, 2);
            if(dist < value){
                value = dist;
            }
        }
    }

    osPthreadCheck(pthread_mutex_lock(&global_lock), "lock");
    if(value < min_dist){
        min_dist = value;
    }
    osPthreadCheck(pthread_mutex_unlock(&global_lock), "unlock");

    return NULL;
}

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    int k = MAX_SIZE;
    points = malloc(k*sizeof(double*));
    check_error(points != NULL, "malloc");
    for(int i = 0; i < k; i++){
        points[i] = malloc(2*sizeof(double));
        check_error(points[i] != NULL, "malloc");
    }

    int i = 0;
    while(scanf("%lf%lf", &points[i][0], &points[i][1]) == 2){
        i++;
        if(i == k - 1){
            k *= 2;
            points = realloc(points, k*sizeof(double*));
            check_error(points != NULL, "realloc");
            for(int j = 0; j < k; j++){
                points[j] = malloc(2*sizeof(double));
                check_error(points[j] != NULL, "malloc");
            }
        }
    }
    numOfStr = i;

    pthread_t* tids = malloc(numOfStr*sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    OsThreadArg* data = malloc(numOfStr*sizeof(OsThreadArg));
    check_error(data != NULL, "malloc");

    osPthreadCheck(pthread_mutex_init(&global_lock, NULL), "init");

    for(int i = 0; i < numOfStr; i++){
        data[i].x = points[i][0];
        data[i].y = points[i][1];
        data[i].row = i;
        osPthreadCheck(pthread_create(&tids[i], NULL, osThreadFunc, &data[i]), "create");
    }

    for(int i = 0; i < numOfStr; i++){
        osPthreadCheck(pthread_join(tids[i], NULL), "join");
    }

    osPthreadCheck(pthread_mutex_destroy(&global_lock), "destroy");

    printf("%lf\n", sqrt(min_dist));

    for(int i = 0; i < numOfStr; i++){
        free(points[i]);
    }
    free(points);
    free(tids);
    free(data);

    exit(EXIT_SUCCESS);
}
