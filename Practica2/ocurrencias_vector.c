#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//variables globales
int cant_ocurrencias_paralelo=0,nro_encontrar;
int* vec;
struct pos{
    int primera;
    int ultima;
};

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
/*     int i;
    struct pos *p;
    p=(struct pos*) ptr;
    struct pos posiciones = *p;
    int primera = posiciones.primera;
    int ultima = posiciones.ultima;

     for(i=primera;i<ultima;i++){
        if(vec[i]==nro_encontrar){
            pthread_mutex_lock(&acceder_var);
            cant_ocurrencias_paralelo++;
            pthread_mutex_unlock(&acceder_var);
        }
    } */
    printf("buenos dias");
    pthread_exit(0);
}


int main(int argc, char *argv[]){
    double timeSecuencial,timeParalelo,tick;
    int i,j,k,cant_threads,N,cant_ocurrencias_secuencial=0;
    pthread_attr_t attr;

    if (argc != 4){
        printf("El primer parametro debe ser el N, el segundo la cantidad de hilos y el tercero el nro a encontrar");
        exit(1);
    }
    N = atoi(argv[1]);
    cant_threads = atoi(argv[2]);
    nro_encontrar = atoi(argv[3]);
    struct pos posiciones[cant_threads]; //para mandarle lo que debe recorrer
    pthread_t threads[cant_threads]; //declaracion de los threads

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

    //calculo cant de elementos, para cada thread
    int cant_elem = N/cant_threads;
    int aux=0;
    for(i=0;i<cant_threads;i++){
        posiciones[i].primera=aux;
        aux+=cant_elem;
        posiciones[i].ultima=aux-1;
        printf("Thread: %i, primera:%i, ultima:%i \n",i,posiciones[i].primera,posiciones[i].ultima);
    }
    tick = dwalltime();

    for(i=0;i<cant_threads;i++){
        pthread_create(&threads[i],&attr,calcular_ocurrencias,&posiciones[i]);
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