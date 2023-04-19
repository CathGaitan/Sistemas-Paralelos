#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>

int* vec;
int maxP,minP,sumP,cant_threads,N;

//semaforos
sem_t sem_max,sem_min,sem_sum;


//Para calcular tiempo
double dwalltime(){
	double sec;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}

void * buscar_max_min_prom(void* ptr){
    int *p,id,i,sum;
    p=(int*) ptr;
    id=*p;
    int primera=id*(N/cant_threads);
    int ultima=primera+(N/cant_threads)-1;
    int max=vec[primera],min=vec[primera];
    //guardar en var local y al final actualiza
    //printf("Thread %i, primera:%i, ultima:%i\n",id,primera,ultima);
    for(i=primera;i<=ultima;i++){
        if(vec[i]>max) max=vec[i];
        else if(vec[i]<min) min=vec[i];
        sum+=vec[i];
    }
    //printf("Thread:%i, max:%i,min:%i\n",id,max,min);
    sem_wait(&sem_max);
    if(maxP<max) maxP=max;
    sem_post(&sem_max);

    sem_wait(&sem_min);
    if(minP>min) minP=min;
    sem_post(&sem_min);

    sem_wait(&sem_sum);
    sumP+=sum;
    sem_post(&sem_sum);

    pthread_exit(0);
}

int main(int argc,char *argv[]){
    double timeSecuencial,timeParalelo,tick;
    int i;
    pthread_attr_t attr;

    //Verificar parametro 
    if (argc != 3){
        printf("El primer parametro debe ser el N, el segundo la cantidad de hilos");
        exit(1);
    }
    N = atoi(argv[1]);
    cant_threads=atoi(argv[2]);

    pthread_t threads[cant_threads];
    int ids[cant_threads];
    vec=(int*)malloc(sizeof(int)*N);
    pthread_attr_init(&attr); //inicializacion del atributo

    for(i=0;i<N;i++){
        vec[i]=rand()%100+1;
    }

//-----------------------------------------------------------------------------
    //Calculo secuencial
    int maxS=vec[0],minS=vec[0],sumS=0;
    double promS;

    tick = dwalltime();
    for(i=0;i<N;i++){
        if(vec[i]>maxS) maxS=vec[i];
        else if(vec[i]<minS) minS=vec[i];
        sumS+=vec[i];
    }  
    promS=sumS/N;
    timeSecuencial = dwalltime() - tick;
    printf("Secuencial: Max:%i, Min:%i, Prom:%f\n",maxS,minS,promS);
    printf("Tiempo requerido secuencial: %f\n",timeSecuencial);

//-----------------------------------------------------------------------------
    //inicializacion semaforos
    sem_init(&sem_max,1,1); 
    sem_init(&sem_min,1,1);
    sem_init(&sem_sum,1,1);

    //Calculo paralelo
    tick = dwalltime();
    maxP=vec[0];minP=vec[0];sumP=0;
    for(i=0;i<cant_threads;i++){
        ids[i]=i;
        pthread_create(&threads[i],&attr,buscar_max_min_prom,&ids[i]);
    }

    for(i=0;i<cant_threads;i++){
        pthread_join(threads[i],NULL);
    }
    double promP=sumP/N;

    timeParalelo = dwalltime() - tick;
    printf("Paralelo: Max:%i, Min:%i, Prom:%f\n",maxP,minP,promP);
    printf("Tiempo requerido paralelo con %i threads: %f\n",cant_threads,timeParalelo);
    free(vec);

}