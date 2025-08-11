// Program dobija rec w koja bi treba da sadrzi samo velika slova (proveriti).
// Ispisati vrednosti svih promenjivih okruzenja takvih da pocinju sa w.
#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define check_error(cond, msg)\
    do{\
        if(!(cond)){\
            perror(msg);\
            exit(EXIT_FAILURE);\
        }\
    } while(0)

int all_caps(char* str){

    for(int i = 0; str[i]; i++){
        if(!(isalpha(str[i]) && isupper(str[i]))){
            return 0;
        }
    }

    return 1;
}


// Ovako ukljucujemo environ - okruzenje
extern char** environ;

void print_env(char* word){

    char** ep;
    int n = strlen(word);
    // n + 1 posto nam treba 1 mesto za terminirajucu nulu, posto nam okruzenje
    // ne garantuje da cemo je imati u nisci
    char str[n + 1];
    // Okruzenje je pokazivac na niz pokazivaca na niske i on ide sve do NULL.
    for(ep = environ; *ep != NULL; ep++){
        // Kopiramo ime orkuzenja
        strncpy(str, *ep, n);
        // Proveravamo da li okruzenje isto
        if(!strcmp(str, word)){
            // Ako jeste trazimo pojavljivanje =, i posto njega ne ispisujemo
            // sa tmp + 1 pomeramo pokazivac na ime promenjive koje nam treba
            char*  tmp = strchr(*ep, '=');
            printf("%s ", tmp + 1);
        }
    }

}

int main(int argc, char** argv){

    check_error(argc == 2, "Argumenti");

    char* word = argv[1];
    check_error(all_caps(word), "Lose uneta rec");

    print_env(word);

    exit(EXIT_SUCCESS);
}
