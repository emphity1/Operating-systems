#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#define MAX 22
/*
    programma che in input riceve N file, crea N thread che leggono ognuno un file, cercando un CHAR, contando 
    quante volte si ripete e copiano tutti M CHAR che ha trovato in un nuovo file.

*/
char *buffer[2] = {"ciao.txt","monster.txt"};
char buff[MAX];
char tmp_buffer[MAX];
int file_length = 0;
char buffer_finale[MAX];
int nt = 0;
int count = 0;

pthread_mutex_t mutex;

void *worker(void *param){
    
    int id = *(int*) param; //id del mio thread
    int fl=0;
    size_t currLetto = 0;
    size_t letto = 0;
    size_t rb = 0;
    printf("\nSono processo numero %d", id);
    int res = 0;

    for(int i=0;i<=nt;i++){ //N-simo thread legge N-simo file dal buffer
        if(i==id){//per ogni N-simo thread...
        //apro il file
        int fd = open(buffer[i],O_RDONLY,0660);
        if(fd<0){
            printf("\nErrore di lettura");
            exit(0);
        }
        printf("\nBuffer è %s",buffer[i]);
        pthread_mutex_lock(&mutex);
        fl = lseek(fd,0,SEEK_END); //lunghezza del N-simo file
        lseek(fd,0,SEEK_SET);
        
        printf("\nfile length è %d", fl);
        //leggo il file
        while(currLetto<fl){
            letto = read(fd,buff,fl);
            currLetto++;
            }

        //cerco il CHAR desiderato
        off_t slice = fl; //il mio slice è tutta la lunghezza del file
        for(int i=0;i<slice;i++){
            size_t inizio = 0;//comincio da 0...
            inizio +=i;//aumento ogni byte...
            if(buff[inizio]=='b'){//..e verifico che quel byte corrisponda al mio CHAR
                tmp_buffer[i] = buff[inizio]; //utilizzo un temp buffer per memorizzare tutti CHAR trovati.
                res++; //conto quante volte si ripete
            }
            
        }
        
        strcat(buffer_finale,tmp_buffer);//concateno al buffer_finale, il mio temp_buffer con i miei CHAR.
        close(fd);
        pthread_mutex_unlock(&mutex);
        }
    }
   

   //alla fine dei lavori prendo il mio contatore locale e lo spiaccico nel count che e' variabile globale
   __atomic_fetch_add(&count, res, __ATOMIC_SEQ_CST); 
   return NULL;
}

int main(int argc, char *argv[]){


    nt = argc-1;
    printf("\nHai inserito %d file\n",nt);
    int i=0;
    int fdo;
    size_t wb = 0;

   //creo thread
   int *index = malloc(nt*sizeof(pthread_t));
   pthread_t *tids = malloc(nt*sizeof(pthread_t));
   
   pthread_mutex_init(&mutex,NULL);
   for(i=0;i<nt;i++){
        index[i] = i;
        pthread_create(tids+i,NULL,worker,index+i);
    }
    for(i=0;i<nt;i++){ 
        pthread_join(tids[i],NULL);
    }

    pthread_mutex_destroy(&mutex);

    //copio CHAR trovati in un nuovo file.
    printf("\n\nHo contato in  totale %d caratteri\n\n", count);
    printf("\nBuffer finale: %s\n\n",buffer_finale);
    sleep(1);
    printf("\nCopio il char trovato nel nuovo file...\n");


    fdo = open("final.txt",O_WRONLY,0660);
    wb = write(fdo,buffer_finale,count);
    if(wb<0){
        printf("\nErrore in scrittura...");
        exit(0);
    }

    close(fdo);
}