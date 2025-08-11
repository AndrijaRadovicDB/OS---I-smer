// Program obradjuje fifo fajlove preko epoll API. Na kraju ispisuje najveci broj
// koji je procitao i ime fifo fajla u kome ga je procitao.
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

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

#define MAX_SIZE 256

typedef struct{
    int fd;
    char* fName;
} data_t;

int main(int argc, char** argv){

    check_error(argc > 1, "Argumenti");

    int numFds = argc - 1;
    int epollFd = epoll_create(numFds);
    check_error(epollFd != -1, "epoll_create");
    struct epoll_event* fds = malloc(numFds*sizeof(struct epoll_event));
    check_error(fds != NULL, "malloc");

    for(int i = 0; i < numFds; i++){
        int fd = open(argv[i + 1], O_RDONLY | O_NONBLOCK);
        check_error(fd != -1, "open");

        data_t* data = malloc(sizeof(data_t));
        check_error(data != NULL, "malloc");
        data->fd = fd;
        data->fName = argv[i + 1];

        fds[i].events = EPOLLIN | EPOLLERR | EPOLLHUP;
        fds[i].data.ptr = data;
        check_error(epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &fds[i]) != -1, "epoll_ctl");
    }

    int actFds = numFds;
    int max_num = INT_MIN;
    char* name;
    struct epoll_event actEv[20];

    while(actFds){
        int ret_val = epoll_wait(epollFd, actEv, 20, -1);
        check_error(ret_val != -1, "epoll_wait");
        for(int i = 0; i < ret_val; i++){
            if(actEv[i].events & EPOLLIN){
                data_t* data = (data_t*)actEv[i].data.ptr;
                FILE* f = fdopen(data->fd, "r");
                int num;
                while(fscanf(f, "%d", &num) == 1){
                    if(num > max_num){
                        max_num = num;
                        name = data->fName;
                    }
                }
            }
            else if(actEv[i].events & (EPOLLERR | EPOLLHUP)){
                data_t* data = (data_t*)actEv[i].data.ptr;
                check_error(epoll_ctl(epollFd, EPOLL_CTL_DEL, data->fd, &actEv[i]) != -1, "epoll_ctl");
                close(data->fd);
                actFds--;
                free(data);
            }
        }
    }

    printf("%s %d\n", name, max_num);

    free(fds);

    exit(EXIT_SUCCESS);
}
