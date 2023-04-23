#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

int *conjuntoA,*conjuntoB;
int N,cant_threads;

//Para calcular tiempo
double dwalltime(){
	double sec;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}

void * calcular_algo(void* ptr){

}


int main(int argc, char *argv[]){
    double timeSecuencial,timeParalelo,tick;
    int i;
    pthread_attr_t attr;

    //Verificar parametro 
    if (argc != 3){
        printf("El primer parametro debe ser el N, y el segundo la cantidad de hilos\n");
        exit(1);
    }
    N = atoi(argv[1]);
    cant_threads = atoi(argv[2]);  
    int ids[cant_threads]; 
    pthread_t threads[cant_threads]; //declaracion de los threads

    conjuntoA=(int*)malloc(sizeof(int)*N);
    conjuntoB=(int*)malloc(sizeof(int)*N);
    pthread_attr_init(&attr); //inicializacion del atributo


    //calculo secuencial


    //calculo paralelo
    for(i=0;i<cant_threads;i++){
        ids[i]=i;
        pthread_create(&threads[i],&attr)
    }
}