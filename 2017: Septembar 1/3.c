// Program prima relativne putanje do FIFO fajlova, sa epoll API istovremeno
// motri pve fajlove cekajuci na dogadjaj citanja. Program cita realne brojeve
//iz FIFO fajlova sve dok ima brojeva i ispisuje relatvinu putanju do FIFO fajla
// ciji je procitanih brojeva najveci
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <sys/epoll.h> // Kao poll, sluzi pri radu sa I/O eventovima

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

typedef struct{
    int fd;
    char* fName;
} data_t;

int main(int argc, char** argv){

    check_error(argc > 1, "Argumenti");

    // Broj FIFO fajlova
    int numFds = argc - 1;
    int epollFd = epoll_create(numFds);
    // epoll_create - pravi novu epoll instancu i vraca fd na nju
    check_error(epollFd != -1, "epoll_create");
    // Ova struktura sadrzi evente i strukturu data, cuva pokazivac i jos nesto
    struct epoll_event* fds = (struct epoll_event*)malloc(numFds*sizeof(struct epoll_event));
    check_error(fds != NULL, "malloc");

    // Otvaranje svih fajlova, njihova obrada i njihovo dodavanje na epoll API
    for(int i = 0; i < numFds; i++){
        int fd = open(argv[i + 1], O_RDONLY | O_NONBLOCK);
        check_error(fd != -1, "open");

        data_t* data = (data_t*)malloc(sizeof(data_t));
        check_error(data != NULL, "malloc");

        data->fd = fd;
        data->fName = argv[i + 1];

        fds[i].events = EPOLLIN | EPOLLERR | EPOLLHUP;
        fds[i].data.ptr = data;
        // epoll_ctl dodaje odredjen fd na listu epoll instance
        // EPOLL_CTL_ADD - dodaje fd iz naseg API sa odredjenim eventima iz fds
        check_error(epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &fds[i]) != -1, "epoll_ctl");
    }

    // Broj aktivnih fajlova
    int actNum = numFds;
    float maxSum = 0;
    char* name;
    struct epoll_event actEv[20];

    while(actNum){
        // epoll_wait blokira pozivanje niti ako nema nikakvih eventa trenutno
        // 20 - maks eventova, -1 tj 0 da nema cekanja
        int retVal = epoll_wait(epollFd, actEv, 20, -1);
        // Kod poll idu reventi, a zbor epollwait kod epoll idu eventi
        check_error(retVal != -1, "epoll_wait");
        for(int i = 0; i < retVal; i++){
            if(actEv[i].events & EPOLLIN){
                data_t* data = (data_t*)actEv[i].data.ptr;
                FILE* f = fdopen(data->fd, "r");
                float sum = 0;
                float x;
                while(fscanf(f, "%f", &x) != EOF){
                    sum += x;
                }
                if(maxSum == 0 || sum > maxSum){
                    maxSum = sum;
                    name = data->fName;
                }
            }
            else if(actEv[i].events & (EPOLLHUP | EPOLLERR)){
                data_t* data = (data_t*)actEv[i].data.ptr;
                // EPOLL_CTL_DEL - sklanja fd sa API-a
                check_error(epoll_ctl(epollFd, EPOLL_CTL_DEL, data->fd, &actEv[i]) != -1, "epoll_ctl");
                close(data->fd);
                actNum--;
                free(data);
            }
        }
    }
    printf("%s\n", name);

    free(fds);

    exit(EXIT_SUCCESS);
}
