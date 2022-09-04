#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <stdbool.h>
#include <string.h>




int nt; //numero dei thread
int atomic_file_length = 0;
int queue[20];
char* bufferA;
int count = 0;
int q_file = 0;


void* worker(void*param){
    int res = 0;
    int id = *(int*)param;
    int i=0;
    
    printf("Sono thread num: %d \n", id);
    
    for(i=0; i<4;i++){
        int slice = (queue[i]+queue[i+1]);
        int inizioBase = slice*id;
        
        printf("Queue [%d]:%d, Slice %d\n",i,queue[i],slice);
        if(queue[i]==queue[i+1]){
              res++;
        }
        
    }
    
    __atomic_fetch_add(&count, res, __ATOMIC_SEQ_CST); 
    
}

void confronta(int num_t){
    int i;
    int *index = (int*)malloc(num_t*sizeof(int));
    pthread_t* tids = malloc(num_t*sizeof(pthread_t));

    for(i=0;i<num_t;i++){
        index[i]=i;
        pthread_create(tids+i,NULL,worker,index+i);
    }

    for(i=0;i<num_t;i++){
        pthread_join(tids[i],NULL);
    }


}

int main(int argc,char* argv[]){

    //quantita' di file da confrontare
    int qf = argc-1;
    printf("Hai inserito %d file\n",qf);
    int i;
    int fd_o,fd_r;
    int size_r, letto;
    if((qf %2)==0){
        nt = qf/2; //ogni coppia di file avra' un thread
    }else{
        printf("Devi inserire un numero di file pari!\n");
        exit(0);
    }

    __atomic_fetch_add(&q_file, qf, __ATOMIC_SEQ_CST);
    

    bufferA = (char*)malloc(qf*sizeof(char));

    int file_length = 0;
    int filesize[qf+1];
    int fs = 0;
    for(i=1;i<=qf;i++){
        fd_o= open(argv[i],O_RDONLY);
        fs = lseek(fd_o,0,SEEK_END);
        filesize[i] = fs;
        if(fd_o<0){
            printf("Il file - %s - non puo essere letto!\n ", argv[i]);
            exit(0);
        }
        file_length = file_length + fs;
        fs = lseek(fd_o,0,SEEK_SET);
        
        printf("____________________________________________________________\n");
        printf("File inserito %s piu la somma dei precedenti e' %d\n", argv[i],file_length);
        printf("Chunk e' lungo %d \n\n ",filesize[i]);
        __atomic_fetch_add(&queue[i], filesize[i], __ATOMIC_SEQ_CST); 
        
    }
    

    __atomic_fetch_add(&atomic_file_length, file_length, __ATOMIC_SEQ_CST);

    printf("File size di tutti file e': %d \n",file_length);
    
    // printf("Prova variabile atomica1 %d\n", queue[1]);
    // printf("Prova variabile atomica2 %d\n", queue[2]);
    // printf("Prova variabile atomica3 %d\n", queue[3]);
    // printf("Prova variabile atomica4 %d\n", queue[4]);
    sleep(0.5);


    //leggo tutti i file
    int k = 0;
    while(k <= filesize[i]){
        letto = read(fd_o,bufferA,file_length);
        k++;
    }

   


    confronta(nt);

    printf("%d file ha/hanno la stessa lunghezza\n\n",count);

    return 0;
}