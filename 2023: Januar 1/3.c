// Program cita fajl i pokrece nit za svako slovo alfabeta i odredjuje se njegov
// broj pojavljivanja, dobijeni broj se vraca kao povratna vrednost niti. Na
// kraju treba ispisati ukupan broj slova, slovo sa najvise ponavljanja i njegov
// broj pojavljivanja. Za sihnronizaciju se koristi muteks.
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

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

#define THREAD_NUM 26

typedef struct{
    char c;
} OsThreadArg;

typedef struct{
    int c_num;
} OsThreadOutput;

int all_chars = 0;
char* fPath = NULL;
pthread_mutex_t global_lock;

void* OsThreadFunc(void* args){

    OsThreadArg* arg = (OsThreadArg*)args;

    FILE* f = fopen(fPath, "r");
    check_error(f != NULL, "fopen");

    int value = 0;
    char c;
    while((c = fgetc(f)) != EOF){
        if(tolower(c) == arg->c){
            value++;
        }
    }

    fclose(f);

    OsPthreadCheck(pthread_mutex_lock(&global_lock), "lock");
    all_chars += value;
    OsPthreadCheck(pthread_mutex_unlock(&global_lock), "unlock");

    OsThreadOutput* output = malloc(sizeof(OsThreadOutput));
    check_error(output != NULL, "malloc");
    output->c_num = value;

    return output;
}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    fPath = argv[1];
    int k = THREAD_NUM;
    pthread_t* tids = malloc(k*sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    OsThreadArg* data = malloc(k*sizeof(OsThreadArg));
    check_error(data != NULL, "malloc");

    OsPthreadCheck(pthread_mutex_init(&global_lock, NULL), "init");

    for(int i = 0; i < k; i++){
        data[i].c = 'a' + i;
        OsPthreadCheck(pthread_create(&tids[i], NULL, OsThreadFunc, &data[i]), "create");
    }

    char max_char;
    int max_char_num = 0;
    for(int i = 0; i < k; i++){
        OsThreadOutput* data_output = NULL;
        OsPthreadCheck(pthread_join(tids[i], (void**)&data_output), "join");
        if(data_output->c_num > max_char_num){
            max_char = 'a' + i;
            max_char_num = data_output->c_num;
        }
        free(data_output);
    }

    OsPthreadCheck(pthread_mutex_destroy(&global_lock), "destroy");

    printf("%d %c %d\n", all_chars, max_char, max_char_num);

    free(tids);
    free(data);

    exit(EXIT_SUCCESS);
}
