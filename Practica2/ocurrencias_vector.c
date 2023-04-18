#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//variables globales
int cant_ocurrencias_paralelo=0,nro_encontrar,cant_threads,N;
int* vec;

//mutex
pthread_mutex_t acceder_var;

//Para calcular tiempo
double dwalltime(){
	double sec;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}

void * calcular_ocurrencias(void * ptr){
    int *p,id,i,ocurrencias=0;
    p=(int*) ptr;
    id=*p;
    int primera=id*(N/cant_threads);
    int ultima=primera+(N/cant_threads);
    for(i=primera;i<ultima;i++){
        if(vec[i]==nro_encontrar){
            ocurrencias++;
        }
    }
    pthread_mutex_lock(&acceder_var);
    cant_ocurrencias_paralelo+=ocurrencias;
    pthread_mutex_unlock(&acceder_var);
    pthread_exit(0);
}

int main(int argc, char *argv[]){
    double timeSecuencial,timeParalelo,tick;
    int i,j,k,cant_ocurrencias_secuencial=0;
    pthread_attr_t attr;

    if (argc != 4){
        printf("El primer parametro debe ser el N, el segundo la cantidad de hilos y el tercero el nro a encontrar");
        exit(1);
    }
    N = atoi(argv[1]);
    cant_threads = atoi(argv[2]);
    nro_encontrar = atoi(argv[3]);
    pthread_t threads[cant_threads]; //declaracion de los threads
    int ids[cant_threads];

    vec=(int*)malloc(sizeof(int)*N);

    for(i=0;i<N;i++){
        if(i==2 || i==6 || i==8) vec[i]=99;
        else vec[i]=i;
    }

    //calculo secuencial
    tick = dwalltime();
    for(i=0;i<N;i++){
        if(vec[i]==nro_encontrar){
            cant_ocurrencias_secuencial++;
        }
    }

    timeSecuencial = dwalltime() - tick;
    printf("Tiempo para calcular cantidad de ocurrencias secuencial: %f\n",timeSecuencial);
    printf("Cantidad de ocurrencias en secuencial: %i \n",cant_ocurrencias_secuencial);

    //Calculo paralelo
    tick = dwalltime();

    for(i=0;i<cant_threads;i++){
        ids[i]=i;
        pthread_create(&threads[i],&attr,calcular_ocurrencias,&ids[i]);
    }

    for(i=0;i<cant_threads;i++){
        pthread_join(threads[i],NULL); //espero a que terminen todos los hilos
    }
    
    timeParalelo = dwalltime() - tick;
    printf("Tiempo requerido para calcular la multiplicacion paralela con %i threads: %f\n",cant_threads,timeParalelo);
    printf("Cantidad de ocurrencias en paralelo: %i\n",cant_ocurrencias_paralelo);
    free(vec);
    exit(0);
}
