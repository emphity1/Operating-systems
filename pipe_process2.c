#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
/*
Il child chiede in input una stringa,
e la manda tramite pipe al parent.
*/

int main(){

    int fd[2];
    
    if(pipe(fd)==-1){return 1;}

    int pid = fork();

    if(pid ==0){//child
        close(fd[0]); //non devo leggere nulla dalla pipe

        char str[200];
        
        printf("Input string: ");
        fgets(str,200,stdin);
        str[strlen(str)-1] = '0'; //rimuovo ultimo carattere della stringa
        int n = strlen(str)+1; //lunghezza della mia stringa

        write(fd[1],&n, sizeof(int)); //passo alla pipe quanto e' lunga la mia str
        write(fd[1],str,sizeof(char) * n);
        close(fd[1]); //ho finito di scrivere
    
    }else{
        close(fd[1]); // non scrivo' nella pipe
        char str[200];
        int n;

        read(fd[0],&n,sizeof(int));
        read(fd[0],str,sizeof(char)*n);
        printf("Ricevuti: %s\n", str);
        close(fd[0]);
        wait(NULL);
    }

    return 0;
}
