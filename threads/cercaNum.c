#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <stdbool.h>


int nt;
size_t filesize;
int* buffer;


void* routine(void* param) {
    int count = 0;
    int id = *(int*)param;
    off_t slice = filesize/nt;
    off_t remain = filesize%nt;

    off_t mySlice = slice;

    //bite dall'inizio del thread
    size_t inizioBase = slice*id;
    int i;
    
    for(i=0;i<mySlice;i++){
         
        size_t inizio = inizioBase;
        inizio +=i;
        if(buffer[inizio]==2){
            count++;
            
        }else{
            i++;
        }
         
    }
    printf("Cosa stamp %d\n",count);
    *(int*)param = count;
    return param;
}

int main(int argc, char* argv[]) {


    int ifd;
    int size_r = 0;
    ifd=open(argv[1],O_RDONLY);
    nt = atoi(argv[2]);

    filesize = lseek(ifd,0,SEEK_END);
    lseek(ifd,0,SEEK_SET);

    buffer = (int*)malloc(sizeof(filesize));

    //leggo tutto il file
    while(size_r<filesize){
        size_t letto = read(ifd,buffer,filesize);
        size_r += letto;
    }

    int *a = (int*)malloc(nt*sizeof(int));
    pthread_t* tids = malloc(nt*sizeof(pthread_t));
    
    int i;
    for (i = 0; i < nt; i++) {
        a[i] = i;
        if (pthread_create(&tids[i], NULL, &routine, a+i) != 0) {
            perror("Failed to create thread");
        }
    }
    int globalSum = 0;
    for (i = 0; i < nt; i++) {
        int* r;
        if (pthread_join(tids[i], (void**) &r) != 0) {
            perror("Failed to join thread");
        }
        globalSum += *r;
        
    }
    
    printf("Global sum: %d\n", globalSum);
    return 0;
}