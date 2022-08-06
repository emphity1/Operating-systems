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
    //gen casuale di array
    int arr[5];
    srand(time(NULL));
    for (int i =0;i<5;i++){
        arr[i] = rand() % 100;
    }

    printf("Sto Aspettando...\n");
    int fd = open("sum", O_WRONLY);
    
    if(fd==-1){
        return 1;
    }

    for(int i=0; i<5;i++){
        if(write(fd, &arr[i], sizeof(int))==-1){
            return 2;
        }
        printf("Wrote %d\n", arr[i]);
        
    }
  
    close(fd);

    int somma =0;
    for(int i=0; i<5; i++){
        somma += arr[i];
    }
    printf("La somma dei numeri e': %d ", somma);

    return 0;
}