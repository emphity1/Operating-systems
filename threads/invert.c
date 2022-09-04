/*ESERCIZIO 3:  
  PRENDE DA LINEA DI COMANDO I FILE 'A' E 'B' E PRENDE ANCHE IL NUMERO DI THREAD/PROCESSI N!!!!:  
  IL FILE A E' IL FILE SORGENTE MENTRE IL FILE B E LA DESTINAZIONE  
  OBIETTIVO: INVERTIRE A E METTERE IL CONTENUTO INVERTITO IN B IL TUTTO FATTO CONCORRENTEMENTE!!!*/ 
 
#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <fcntl.h> 
#include <unistd.h> 
 
#define abort(msg) do{printf(msg);exit(1);}while(0);  
 
char* bufferInput; 
char* bufferOutput; //i buffer 
size_t file_size; 
int numero_thread; 
 
void* funz(void* argm){ 
 int id = *(int*)argm; 
 
 /*taglie da leggere*/ 
 off_t slice = file_size/numero_thread; 
 off_t remain = file_size%numero_thread; 
 /*del thread*/ 
 off_t mySlice = slice;  
 if(id == numero_thread-1) mySlice+=remain;  
 
 /*byte d'inizio del thread*/ 
 size_t inizioBase = slice * id;  
 int i;  
 
 /*inverto la mia porzione di buffer*/ 
 for(i = 0; i < mySlice; i++){ 
  size_t inizio = inizioBase; 
  inizio += i;  
  size_t destinazione = file_size - 1 - inizio; 
  bufferOutput[destinazione] = bufferInput[inizio]; 
 } 
 
 printf("[%d]: invertito da %ld a %d!\n",id,mySlice*id,i);  
}  
 
void invert(int numero_thread){ 
 
// int argm[numero_thread]; 
// pthread_t tids[numero_thread];  
 int* argm = (int*)malloc(numero_thread * sizeof(int)); 
 pthread_t* tids = malloc(numero_thread * sizeof(pthread_t));  
 
 /*crea i thread*/ 
 int i;  
 for(i = 0; i < numero_thread; i++){ 
  argm[i] = i;  
//  pthread_create(&tids[i],NULL,funz,argm[i]);  
  pthread_create(tids+i,NULL,funz,argm+i);  
 } 
 for(i = 0; i < numero_thread; i++) { 
 // pthread_join(tids[i],NULL);  
  pthread_join(*(tids+i),NULL);  
 } 
} 
 





int main(int argc, char const *argv[]) 

{ 
 int ifd,ofd,size_r,size_w;  
 
 /*controllo argomenti*/  
 if(argc != 4)abort("[ERR] : use -> ./eser3 <nome_file_A> <nome_file_B> <numero_thread>\n");  
 
 numero_thread = atoi(argv[3]);  
 
 if((ifd = open(argv[1],O_RDONLY)) == -1)abort("[ERR] : apertura file A!\n");  
 if((ofd = open(argv[2],O_WRONLY | O_CREAT | O_TRUNC,0660)) == -1)abort("[ERR] : apertura file B!\n"); 
 
  /*ALLOCO I BUFFER*/ 
  file_size = lseek(ifd,0,SEEK_END); //CALCOLO LA DIM DEL FILE 
  /*RIMETTO A POSTO IL PUNTATORE*/ 
  lseek(ifd,0,SEEK_SET);  
 
 bufferInput = (char*)malloc(file_size+1);  
 bufferOutput = (char*)malloc(file_size+1);  
 
 /*LEGGO TUTTO A*/ 
 while(size_r < file_size){ 
  size_t currLetto = read(ifd,bufferInput+size_r,file_size);  
  /*provo*/ 
  if(currLetto < 0)abort("Err lettura!\n");  
  size_r += currLetto;  
 } 
 
 /*INVERTO*/ 
 invert(numero_thread);  
 
 /*SCRIVO SU B*/ 
 while(size_w < file_size){ 
  size_t currScritto = write(ofd,bufferOutput+size_w,file_size);  
  /*provo*/ 
  if(currScritto < 0)abort("Err scrittura!\n");  
  size_w += currScritto;  
 } 
 
 printf("FINITO!\n");  
 close(ifd); close(ofd);  
 free(bufferOutput); free(bufferInput);  
 
}