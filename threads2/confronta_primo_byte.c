#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <stdbool.h>
#include <string.h>


/*Si scriva un programma C che prende come argomenti una sequenza di nomi di file.

Per ciascuna coppia di file, il programma crea un nuovo thread/processo il quale:
• verifica se i due possono essere letti e se i rispettivi byte in posizione 0 sono uguali tra loro;
• scrive su standard output il nome dei due file analizzati e l’esito del controllo.
Il programma prima di terminare scrive su standard output il numero di coppie di file che hanno
superato il controllo.*/

pthread_mutex_t mutex;

int nt = 0; //thread number
char *input_buffer[] = {"ciao.txt","random.txt"};
int fd1 =0;
int fd2 =0;
int res = 0;
int uguali = 0;


void* worker(void*param){
    pthread_mutex_lock(&mutex);
    if ( ((fd1=open(input_buffer[0],O_RDONLY))>0) && ((fd2=open(input_buffer[1],O_RDONLY))>0) )
        {   
            char* char1 = malloc(1);
            char* char2 = malloc(1);
            if( (read(fd1,char1,1)>0) && (read(fd2,char2,1)>0)){
                res = 1;
            }else{
                res = 0;
            }

            uguali = char1[0] == char2[0];
        }


    printf("\nFile possono essere letti:%d",res);
    printf("\nPrmo char di entrambi file sono: %d",uguali);
    pthread_mutex_unlock(&mutex);
}




int main(){

    pthread_mutex_init(&mutex, NULL);
	nt = 1;
	//creo i thread
	pthread_t *tids = malloc(nt*sizeof(pthread_t)); 

	for(int i = 0; i < nt; i++) {
		pthread_create(tids+i,NULL,worker, NULL); 
	}
	for (int i = 0; i < nt; ++i)
	{
		pthread_join(*(tids+i),NULL); 
	}

	pthread_mutex_destroy(&mutex);



}