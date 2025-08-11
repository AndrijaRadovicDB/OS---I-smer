// Program prima broj niti i putanju do fajla. Ako je velicina fajla M bajtova
// svaka nit obradjuje M/n bajtova. Svaka nit u trazi niske tag, taa, tga i za
// svako pojavljivanje neke od njih dodaje poziciju pocetka u globalni niz. Na
// kraju ispisati globalni niz
#include <sys/stat.h>
#include <sys/types.h>
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

#define osPthreadCheck(pthreadErr, msg)\
    do{\
        int _pthreadErr = pthreadErr;\
        if(_pthreadErr > 0){\
            errno = _pthreadErr;\
            check_error(false, msg);\
        }\
    } while(0)

#define MAX_COUNT (2048)

typedef struct{
    char* thPath;
    int position;
    int len;
} osThreadArg;

int array[MAX_COUNT];
int len = 0;
char* stop_codon1 = "tag";
char* stop_codon2 = "taa";
char* stop_codon3 = "tga";
pthread_mutex_t global_lock;

void* osThreadFunc(void* args){

    osThreadArg* arg = (osThreadArg*)args;

    int fd = open(arg->thPath, O_RDONLY);
    check_error(fd != -1, "open");

    // lseek pomera fajl ofset fajla datog sa fd za arg->position  u odnosu
    // na SEEK_SET - offset bytes
    check_error(lseek(fd, arg->position, SEEK_SET) != -1, "lseek");

    char buffer[arg->len + 1];
    check_error(read(fd, buffer, arg->len) != -1, "read");
    buffer[arg->len] = '\0';

    int pos[MAX_COUNT];
    int p = 0;
    for(int i = 0; i < arg->len; i++){
        if(!strncmp(&buffer[i], stop_codon1, 3) || !strncmp(&buffer[i], stop_codon2, 3) || !strncmp(&buffer[i], stop_codon3, 3)){
            // Na position pocinje niska koju obradjujemo ali ne i pocetak stop
            // kodona, one je i pozicija posle pocetka (ako postoji)
            pos[p++] = arg->position + i;
        }
    }

    close(fd);

    osPthreadCheck(pthread_mutex_lock(&global_lock), "lock");
    for(int i = 0; i < p; i++){
        array[len++] = pos[i];
    }
    osPthreadCheck(pthread_mutex_unlock(&global_lock), "unlock");

    return NULL;
}

int main(int argc, char** argv){

    check_error(argc == 3, "Argumenti");

    int n = atoi(argv[1]);

    struct stat fInfo;
    check_error(stat(argv[2], &fInfo) != -1, "stat");
    int M = fInfo.st_size;

    pthread_t* tids = malloc(n*sizeof(pthread_t));
    check_error(tids != NULL, "malloc");
    osThreadArg* data = malloc(n*sizeof(osThreadArg));
    check_error(data != NULL, "malloc");

    osPthreadCheck(pthread_mutex_init(&global_lock, NULL), "init");

    for(int i = 0; i < n; i++){
        data[i].thPath = argv[2];
        data[i].len = M/n;
        data[i].position = i*(M/n);
        osPthreadCheck(pthread_create(&tids[i], NULL, osThreadFunc, &data[i]), "create");
    }

    for(int i = 0; i < n; i++){
        osPthreadCheck(pthread_join(tids[i], NULL), "join");
    }

    osPthreadCheck(pthread_mutex_destroy(&global_lock), "destroy");

    for(int i = 0; i < len; i++){
        printf("%d ", array[i]);
    }
    printf("\n");

    free(tids);
    free(data);

    exit(EXIT_SUCCESS);
}
