#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>

int main(){
//Primo processo genera un numero X, poi X viene mandato al processo 2
// il processo due moltiplica X *4 e lo manda indietro al process 1.

    int p1[2];
    if(pipe(p1)==-1){
        return 1;
    }
    int pid = fork();
    if(pid==-1){return 2;}

    if(pid ==0){
        //child
        int x;
        read(p1[0], &x, sizeof(x));
        printf("Ricevuto %d\n",x);
        x *=4;
        write(p1[1], &x, sizeof(x));
        printf("Scritto! %d\n", x);
    }else{
        //parent, genera un numero
        srand(time(NULL));
        int y = rand()%10;
        write(p1[1], &y, sizeof(y));
        printf("Creato %d \n", y);
        read(p1[0], &y, sizeof(y));
        printf("Risultato finale: %d \n", y);

    }
 
    close(p1[0]);
    close(p1[1]);

    
    return 0;
}