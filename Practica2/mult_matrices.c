#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

int N,cant_threads;
double *A,*B,*C,*D;

//Para calcular tiempo
double dwalltime(){
	double sec;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}

void * mult_matrices(void * ptr){
    int *p,id,i,j,k;
    p=(int*) ptr;
    id=*p;
    int primera=id*(N/cant_threads);
    int ultima=primera+(N/cant_threads);
    for(i=primera; i<ultima; i++){
        for(j=0;j<N;j++){
            double valorD = 0; 
            for(k=0;k<N;k++){
                valorD += A[i*N+k]*B[j*N+k]; 
            }
            D[i*N+j] = valorD;
        }
    }
    pthread_exit(0);
}

int main(int argc, char *argv[]){
    double timeSecuencial,timeParalelo,tick;
    int i,j,k;
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

    //Aloca memoria para las matrices
    A=(double*)malloc(sizeof(double)*N*N);
    B=(double*)malloc(sizeof(double)*N*N);
    C=(double*)malloc(sizeof(double)*N*N);
    D=(double*)malloc(sizeof(double)*N*N);

    //Inicializa las matrices
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            A[i*N+j]=rand() % 2 + 1; //ordenado por fila
            B[j*N+i]=rand() % 2 + 1; //ordenado por columna
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
    printf("Tiempo requerido para calcular la multiplicacion secuencial: %f\n",timeSecuencial);
     printf("imprimo C (primeros 10 elementos) \n");
    for(i=0;i<10;i++){
        for(j=0;j<10;j++){
            printf(" [%i][%i]= %0.0f ",i,j,C[i*N+j]);
        }
        printf("\n");
    }  

    //calculo paralelo
    pthread_attr_init(&attr); //inicializacion del atributo

    tick = dwalltime();

    for(i=0;i<cant_threads;i++){
        ids[i]=i;
        pthread_create(&threads[i],&attr,mult_matrices,&ids[i]);
    }

    for(i=0;i<cant_threads;i++){
        pthread_join(threads[i],NULL); //espero a que terminen todos los hilos
    }

    timeParalelo = dwalltime() - tick;
    printf("Tiempo requerido para calcular la multiplicacion paralela con %i threads: %f\n",cant_threads,timeParalelo);

     printf("imprimo D (primeros 10 elementos) \n");
    for(i=0;i<10;i++){
        for(j=0;j<10;j++){
            printf(" [%i][%i]= %0.0f ",i,j,D[i*N+j]);
        }
        printf("\n");
    }  

    free(A);
    free(B);
    free(C);
    free(D);
}