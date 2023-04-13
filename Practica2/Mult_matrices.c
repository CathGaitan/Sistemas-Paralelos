#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//Para calcular tiempo
double dwalltime(){
	double sec;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}

void * mult_matrices(void * ptr){

}

int main(int argc, char *argv[]){
    double *A,*B,*C;
    double timeSecuencial,timeParalelo,tick;
    int i,j,k,N,cant_threads;
    int id[cant_threads]; //para mandarle el id a cada thread
    pthread_attr_t attr;
    pthread_t threads[cant_threads]; //declaracion de los threads

    //Verificar parametro 
    if (argc != 3){
        printf("El primer parametro debe ser el N, y el segundo la cantidad de hilos");
    }
    N = atoi(argv[1]);
    cant_threads = atoi(argv[2]);

    //Aloca memoria para las matrices
    A=(double*)malloc(sizeof(double)*N*N);
    B=(double*)malloc(sizeof(double)*N*N);
    C=(double*)malloc(sizeof(double)*N*N);

    //Inicializa las matrices reservando posicion en memoria
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            A[i*N+j]=1; //ordenado por fila
            B[j*N+i]=1; //ordenado por columna
        }
    }

    //calculo secuencial
    tick = dwalltime();
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            double valorC = 0; 
            for(k=0;k<N;k++){
                valorC += A[i*N+k]*B[j*N+k]; 
            }
            C[i*N+j] = valorC;
        }
    }
    timeSecuencial = dwalltime() - tick;
    printf("Tiempo requerido para calcular la multiplicacion secuencial: %f\n",time);

    //calculo paralelo
    pthread_attr_init(&attr); //inicializacion
    tick = dwalltime();
    
    for(i=0;i<cant_threads;i++){
        id[i]=i;
        pthread_create(&threads[i],&attr,mult_matrices,&id[i]);
    }

    for(i=0;i<cant_threads;i++){
        pthread_join(threads[i],NULL); //espero a que terminen todos los hilos
    }
    timeParalelo = dwalltime() - tick;
    printf("Tiempo requerido para calcular la multiplicacion paralela con %i threads: %f\n",cant_threads,timeParalelo);

}