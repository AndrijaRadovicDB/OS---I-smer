// Program ucitava broj n, zatim n niski i pokrece n niti. i-ta nit obrce i-tu
// nisku u kroz povratnu vrednost niti vraca obrnutnu nisku. Treba da se formira
// globalna niska nadovezivanjem niski i treba obezbediti zastitu kriticne
// sekcije. Na kraju ispisati povratne vrednosti niti i duzinu globalne niske.
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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

#define MAX_SIZE 256

typedef struct{
    int row;
    int len;
} OsThreadArg;

typedef struct{
    char reverse[MAX_SIZE];
} OsThreadOutput;

char global_str[3*MAX_SIZE];
char** strings = NULL;
pthread_mutex_t global_lock;

void* OsThreadFunc(void* args){

    OsThreadArg* arg = (OsThreadArg*)args;

    for(int j = 0; j < arg->len/2; j++){
        char tmp = strings[arg->row][j];
        strings[arg->row][j] = strings[arg->row][arg->len - j - 1];
        strings[arg->row][arg->len - j - 1] = tmp;
    }

    OsPthreadCheck(pthread_mutex_lock(&global_lock), "lock");
    strcat(global_str, strings[arg->row]);
    OsPthreadCheck(pthread_mutex_unlock(&global_lock), "unlock");

    OsThreadOutput* output = malloc(sizeof(OsThreadOutput));
    check_error(output != NULL, "malloc");
    strcpy(output->reverse, strings[arg->row]);

    return output;
}

int main(int argc, char** argv){

    check_error(argc == 1, "Argumenti");

    int n;
    scanf("%d", &n);
    check_error(n > 0, "Lose n");

    strings = malloc(n*sizeof(char*));
    check_error(strings != NULL, "malloc");
    for(int i = 0; i < n; i++){
        strings[i] = malloc(MAX_SIZE*sizeof(char));
        check_error(strings[i] != NULL, "malloc");
        scanf("%s", strings[i]);
    }

    pthread_t* tids = malloc(n*sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    OsThreadArg* data = malloc(n*sizeof(OsThreadArg));
    check_error(data != NULL, "malloc");

    OsPthreadCheck(pthread_mutex_init(&global_lock, NULL), "init");

    for(int i = 0; i < n; i++){
        data[i].row = i;
        data[i].len = strlen(strings[i]);
        OsPthreadCheck(pthread_create(&tids[i], NULL, OsThreadFunc, &data[i]), "create");
    }

    for(int i = 0; i < n; i++){
        OsThreadOutput* data_output = NULL;
        OsPthreadCheck(pthread_join(tids[i], (void**)&data_output), "join");
        printf("%s ", data_output->reverse);
        free(data_output);
    }
    printf("%ld\n", strlen(global_str));

    OsPthreadCheck(pthread_mutex_destroy(&global_lock), "destroy");

    for(int i = 0; i < n; i++){
        free(strings[i]);
    }
    free(strings);
    free(tids);
    free(data);

    exit(EXIT_FAILURE);
}
