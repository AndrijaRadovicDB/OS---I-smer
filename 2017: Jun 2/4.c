// Program dobija putanje do FIFO fajlova, i on za sve fajlove ceka na dogadjaj da
// je kroz neki FIFO stigao podatak koriscenjem poll interfejsa, kroz fajlove
// dolaze brojevi koje citamo sve dok se ne zatvore svi fajlovi. Na kraju na izlaz
// ispisati najveci procitani broj i NAZIV FIFO fajla
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <poll.h>

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

int main(int argc, char** argv){

    check_error(argc > 1, "Argumenti");

    // Koliko fajl deskriptora imamo
    int numFds = argc - 1;
    // pollfd sadrzi polje za fajl deksriptor, trazene i vracene dogadjaje
    struct pollfd *fds = malloc(numFds*sizeof(struct pollfd));
    check_error(fds != NULL, "malloc");
    FILE** files = malloc(numFds*sizeof(FILE*));
    check_error(files != NULL, "malloc");

    // Otvara sve FIFO fajlove i priprema ih za citanje
    for(int i = 0; i < numFds; i++){
        // O_NONBLOCK - sprecava open da dugo vremena blokiranja otvaranja fajla
        // Koristi se za neke tipove fajlova, ali i da se stavi kad ne treba
        // ne radi nista
        fds[i].fd = open(argv[i + 1], O_RDONLY | O_NONBLOCK);
        check_error(fds[i].fd != -1, "open");
        files[i] = fdopen(fds[i].fd, "r");
        check_error(files[i] != NULL, "fdopen");
        // POLLIN - postoje podaci koji se trebaju procitati
        fds[i].events = POLLIN;
    }

    int numAct = numFds; // Broj aktivnih/otvorenih fajlova
    int max = 0; // Maks broj koji je procitan
    char* name = NULL; // Ime fajla u kom je maks broj
    bool flag = false; // Flag za inicijalizaciju

    while(numAct > 0){
        // poll ceka za neki od fajlova da uradi I/O
        // Zadnji argument je timeout - broj milisekundi koje poll blokira dok
        // ceka za fajl deskriptor da se oslobodi
        // Za timeout 0 nema blokiranja, za -1 nisam siguran vrv isto
        int retVal = poll(fds, numFds, -1);
        check_error(retVal != -1, "poll");
        for(int i = 0; i < numFds; i++){
            // Sa postavljanjem eventova mi menjamo bitove reventa i onda
            // proveravamo da li je dati event ukljucen u revent
            if(fds[i].revents & POLLIN){
                int num;
                while(fscanf(files[i], "%d", &num) == 1){
                    if(!flag){
                        max = num;
                        name = argv[i + 1];
                        flag = true;
                    }
                    else{
                        if(num > max){
                            max = num;
                            name = argv[i + 1];
                        }
                    }
                    // Postavljamo broj na 0, tj nema vise reventova
                    fds[i].revents = 0;
                }
            }
            // POLLER - error condition, POLLHUP - zatvranje druge strane kanala
            // Zatvaramo dati fajl, samim tim vracamo event i revent na 0, fajl
            // deskriptor na -1, a broj aktivnih fajlova smanjujemo za 1
            else if(fds[i].revents & (POLLERR | POLLHUP)){
                close(fds[i].fd);
                fds[i].events = 0;
                fds[i].fd = -1;
                fds[i].revents = 0;
                numAct--;
            }
        }
    }

    char* tmp = strrchr(name, '/');
    name = tmp != NULL ? tmp + 1 : name;
    printf("%d %s\n", max, name);

    free(fds);
    free(files);

    exit(EXIT_SUCCESS);
}
