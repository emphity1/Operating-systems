#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <stdbool.h>
#include <string.h>
#include <stdatomic.h>



/*
Programma per contare quante volte si ripete un carattere nel file con i Thread.
*/

/* VARIABILI */
char* buffer;
int nt;         //numero dei thread
size_t  filesize;
int count = 0;
pthread_mutex_t mutex;


void* worker(void *param){
    int res=0;                          //variabile locale per contare quante volte si ripete un carattere
    int id = *(int*)param;
    off_t slice = filesize/nt;          //quanto e' grande il mio "chunk". File length totale/numero dei thread
    off_t remain = filesize%nt;         //se file e' dispari, questo e' il resto

    off_t mySlice = slice;
    if(id == nt-1) mySlice+=remain;     //all'ultimo thread assegno il resto

    //e' il bite dove iniziare la mia base, il mio chunk
    size_t inizioBase = slice*id;
    int i;
    for(i=0;i<mySlice;i++){             //mentre vado avanti nello slice...
    size_t inizio = inizioBase;
    inizio += i;                        //...aggiorno l'inizio
        if(buffer[inizio]=='a'){        //...e verifico se esiste un certo carattere
          res++;    
        } 
   }
   printf("Ho contato: %d\n", res);
   //alla fine dei lavori prendo il mio contatore locale e lo spiaccico nel count che e' variabile globale
   __atomic_fetch_add(&count, res, __ATOMIC_SEQ_CST); 
   return NULL;
}



void cerca(int num_thread){

    int *ar = (int*)malloc(num_thread*sizeof(int));
    pthread_t* tids = malloc(num_thread*sizeof(pthread_t));

    int i;
    for(i=0;i<num_thread;i++){
        ar[i]=i;
        pthread_create(tids+i,NULL,worker,ar+i);
    }
    for(i=0;i<num_thread;i++){
        pthread_join(tids[i], NULL);
    }
}


int main(int argc,char *argv[]){
    if(argc <= 2){
        printf("Devi inserire 2 pramentri!\n");
        exit(0);
    }
    int ifd;
    int size_r=0;
    ifd = open(argv[1],O_RDONLY);
    nt = atoi(argv[2]);
    
   
    filesize = lseek(ifd,0,SEEK_END);
    lseek(ifd,0,SEEK_SET);

    buffer = (char*)malloc(filesize+1);


    //leggo tutto il file
     while(size_r < filesize){ 
        size_t currLetto = read(ifd,buffer,filesize);  
        size_r += currLetto;  
    }
   
  
    cerca(nt);
    printf("Contato  %d\n", count);
        
    free(buffer);
    close(ifd);

}




