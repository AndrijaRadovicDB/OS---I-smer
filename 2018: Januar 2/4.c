// Program prima putanje do FIFO fajlova, cita karaktere iz njih prateci sve
// fajlove istovremeno preko epoll, ispisati naziv FIFO fajla iz kog je
// procitano najmanje karaktera
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <sys/epoll.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

// Jer je polje data u epoll_event struktura koja cuva informacije o korisniku
// poput pokazivaca na njega, pravimo nasu strukturu koja ce cuvati fd i ime
typedef struct{
    int fd;
    char* fName;
} data_t;

int main(int argc, char** argv){

    check_error(argc > 1, "Argumenti");

    // Priprema epoll interfejsa
    int numFds = argc - 1;
    int epollFd = epoll_create(numFds);
    check_error(epollFd != -1, "epoll_create");
    struct epoll_event* fds = malloc(numFds*sizeof(struct epoll_event));
    check_error(fds != NULL, "malloc");

    // Priprema fajlova
    for(int i = 0; i < numFds; i++){
        int fd = open(argv[i + 1], O_RDONLY | O_NONBLOCK);
        check_error(fd != -1, "open");

        data_t* data = malloc(sizeof(data_t));
        check_error(data != NULL, "malloc");
        data->fd = fd;
        data->fName = argv[i + 1];

        fds[i].events = EPOLLIN | EPOLLERR | EPOLLHUP;
        // Pokazivac na nasu strukturu, sad epoll API ima fd i ime fajla
        fds[i].data.ptr = data;
        check_error(epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &fds[i]) != -1, "epoll_ctl");
    }

    int actFds = numFds;
    int min_ch = INT_MAX;
    char* name;
    struct epoll_event actEv[20];

    while(actFds){
        // epoll_wait vraca broj fd koji su spremi za trazenu I/O operaciju
        int retVal = epoll_wait(epollFd, actEv, 20, -1);
        check_error(retVal != -1, "epoll_wait");
        for(int i = 0; i < retVal; i++){
            if(actEv[i].events & EPOLLIN){
                // Ovo sa datom radimo kako bi dobili fd iz kog citamo
                data_t* data = (data_t*)actEv[i].data.ptr;
                // Ne zatvaramo fajl
                FILE* f = fdopen(data->fd, "r");

                int num_ch = 0;
                char* line = NULL;
                size_t read_bytes = 0;
                while(getline(&line, &read_bytes, f) != -1){
                    num_ch += strlen(line);
                }

                if(num_ch < min_ch){
                    min_ch = num_ch;
                    name = data->fName;
                }

                free(line);

                /* Radi i ovo
                int  num_ch = 0;
                int read_bytes = 0;
                while((read_bytes = read(data->fd, buffer, 4096)) > 0){
                    num_ch += read_bytes;
                } */
            }
            else if(actEv[i].events & (EPOLLERR | EPOLLHUP)){
                // Ovo sa datom radimo kako bi dobili fd koji brisemo sa API
                data_t* data = (data_t*)actEv[i].data.ptr;
                check_error(epoll_ctl(epollFd, EPOLL_CTL_DEL, data->fd, &actEv[i]) != -1, "epoll_ctl");

                close(data->fd);
                actFds--;
                free(data);
            }
        }
    }

    printf("%s\n", name);

    free(fds);

    exit(EXIT_SUCCESS);
}
