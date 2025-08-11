// Program na osnovu prosledjene niske menja prava pristupa programa
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

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

int main(int argc, char** argv){

    check_error(argc == 3, "Argumenti");

    struct stat fInfo;
    check_error(stat(argv[1], &fInfo) != -1, "Nepostojeci fajl");

    char vlasnik = argv[2][0];
    char operacija = argv[2][1];
    char pravo = argv[2][2];

    mode_t stara_prava = fInfo.st_mode;
    mode_t nova_prava = 0;

    if(vlasnik == 'u'){
        if(operacija == '+'){
            if(pravo == 'r')
                nova_prava |= S_IRUSR;
            else if(pravo == 'w')
                nova_prava |= S_IWUSR;
            else
                nova_prava |= S_IXUSR;
        }
        else{
            if(pravo == 'r'){
                nova_prava |= S_IRUSR;
                nova_prava ^= ~0;
            }
            else if(pravo == 'w'){
                nova_prava |= S_IWUSR;
                nova_prava ^= ~0;
            }
            else{
                nova_prava |= S_IXUSR;
                nova_prava ^= ~0;
            }
        }
    }
    else if(vlasnik == 'g'){
        if(operacija == '+'){
            if(pravo == 'r')
                nova_prava |= S_IRGRP;
            else if(pravo == 'w')
                nova_prava |= S_IWGRP;
            else
                nova_prava |= S_IXGRP;
        }
        else{
            if(pravo == 'r'){
                nova_prava |= S_IRGRP;
                nova_prava ^= ~0;
            }
            else if(pravo == 'w'){
                nova_prava |= S_IWGRP;
                nova_prava ^= ~0;
            }
            else{
                nova_prava |= S_IXGRP;
                nova_prava ^= ~0;
            }
        }
    }
    else{
        if(operacija == '+'){
            if(pravo == 'r')
                nova_prava |= S_IROTH;
            else if(pravo == 'w')
                nova_prava |= S_IWOTH;
            else
                nova_prava |= S_IXOTH;
        }
        else{
            if(pravo == 'r'){
                nova_prava |= S_IROTH;
                nova_prava ^= ~0;
            }
            else if(pravo == 'w'){
                nova_prava |= S_IWOTH;
                nova_prava ^= ~0;
            }
            else{
                nova_prava |= S_IXOTH;
                nova_prava ^= ~0;
            }
        }
    }

    if(operacija == '+'){
        stara_prava |= nova_prava;
    }
    else{
        stara_prava &= nova_prava;
    }
    check_error(chmod(argv[1], stara_prava) != -1, "chmod");

    exit(EXIT_SUCCESS);
}
