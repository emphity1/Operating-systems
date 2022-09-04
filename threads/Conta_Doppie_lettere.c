#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#define MAX 100
/*
    Dato il file lorem.txt
    Si devono create N thread a piacere, che contano quante doppie lettere ci sono nel file

*/


int nt = 0;
char *buffer;
size_t file_length;
int res = 0;

void *worker(void *param){
    
    int id = *(int*) param;
    int count=0;
    //divido il file in chunck
    off_t slice=file_length/nt;

    size_t inizioBase = slice*id;
    int i=0;
    for(i=0;i<slice;i++){
        size_t inizio = inizioBase;
        inizio+=i;
        if(buffer[inizio]==buffer[inizio+1]){
            count++;
        }
        
    }
    
   __atomic_fetch_add(&res, count, __ATOMIC_SEQ_CST); 
   //alla fine dei lavori prendo il mio contatore locale e lo spiaccico nel count che e' variabile globale
   //
   return NULL;
}



int main(int argc, char *argv[]){

    nt = argc-1;
    int fd;
    int i;
    fd=open("lorem.txt", O_RDONLY, 0660);
    if(fd<0){
        printf("\nErrore apertura file");
        exit(0);
    }
    printf("\nHo letto il file\n");

    size_t currLetto=0;
    size_t letto=0;

    file_length = lseek(fd,0,SEEK_END);
    if(file_length<0){
        printf("\nImpossibile leggere il file...");
        exit(0);
    }
    lseek(fd,0,SEEK_SET);
    
    printf("File length è %ld\n", file_length);
    
    buffer = (char*)malloc(file_length);

    while(currLetto<file_length){
        letto=read(fd,buffer,file_length);
        currLetto++;
    }
    

    int *index = malloc(nt*sizeof(pthread_t));
    pthread_t *tids = malloc(nt*sizeof(pthread_t));

    for(i=0;i<nt;i++){
        index[i]=i;
        pthread_create(tids+i,NULL,worker,index+i);
    }
    for(i=0;i<nt;i++){
        pthread_join(tids[i],NULL);
    }
    

    printf("\nHo trovato %d coppie di doppie lettere\n",res);
    





   
}