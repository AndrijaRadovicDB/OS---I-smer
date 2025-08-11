// Program ucitava n, pa n brojeva i pokrece n niti. Nit treba da zameni prvu i
// poslednju cifru u broju i dodati taj broj na sumu. Na kraju ipisati novi niz
// i sumu.
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
    int pos;
} OsThreadArg;

int global_sum = 0;
int* array;
pthread_mutex_t global_lock;

int switch_first_last(int num){

    int first = num % 10;
    int new_value = first;

    int m = num, last = 0, len = 1;
    while(m){
        last = m % 10;
        m /= 10;
        len *= 10;
    }
    len /= 10;

    return first*len + (num - last*len - first) + last;
}

void* OsThreadFunc(void* args){

    OsThreadArg* arg = (OsThreadArg*)args;
    int reverse = switch_first_last(array[arg->pos]);

    OsPthreadCheck(pthread_mutex_lock(&global_lock), "lock");
    array[arg->pos] = reverse;
    global_sum += array[arg->pos];
    OsPthreadCheck(pthread_mutex_unlock(&global_lock), "unlock");

    return NULL;
}

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    int n;
    scanf("%d", &n);
    array = malloc(n*sizeof(int));
    check_error(array != NULL, "malloc");
    for(int i = 0; i < n; i++){
        scanf("%d", &array[i]);
    }

    pthread_t* tids = malloc(n*sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    OsThreadArg* data = malloc(n*sizeof(OsThreadArg));
    check_error(data != NULL, "malloc");

    OsPthreadCheck(pthread_mutex_init(&global_lock, NULL), "init");

    for(int i = 0; i < n; i++){
        data[i].pos = i;
        OsPthreadCheck(pthread_create(&tids[i], NULL, OsThreadFunc, &data[i]), "create");
    }

    for(int i = 0; i < n; i++){
        OsPthreadCheck(pthread_join(tids[i], NULL), "join");
    }

    OsPthreadCheck(pthread_mutex_destroy(&global_lock), "destroy");

    for(int i = 0; i < n; i++){
        printf("%d ", array[i]);
    }
    printf("\n%d\n", global_sum);

    free(array);
    free(tids);
    free(data);

    exit(EXIT_SUCCESS);
}
