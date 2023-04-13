#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct pos{
    int primera;
    int ultima;
};
int N;
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
    int i,j,k;
    struct pos *p;
    p=(struct pos*) ptr;
    struct pos posiciones = *p;
    int primera = posiciones.primera;
    int ultima = posiciones.ultima;
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
    int i,j,k,cant_threads;
    pthread_attr_t attr;

    //Verificar parametro 
    if (argc != 3){
        printf("El primer parametro debe ser el N, y el segundo la cantidad de hilos");
    }
    N = atoi(argv[1]);
    cant_threads = atoi(argv[2]);
    
    struct pos posiciones[cant_threads]; //para mandarle lo que debe recorrer
    pthread_t threads[cant_threads]; //declaracion de los threads

    //Aloca memoria para las matrices
    A=(double*)malloc(sizeof(double)*N*N);
    B=(double*)malloc(sizeof(double)*N*N);
    C=(double*)malloc(sizeof(double)*N*N);
    D=(double*)malloc(sizeof(double)*N*N);

    //Inicializa las matrices reservando posicion en memoria
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
    pthread_attr_init(&attr); //inicializacion
    int cant_elem = N/cant_threads;
    int aux=0;
    for(i=0;i<cant_threads;i++){
        posiciones[i].primera=aux;
        aux+=cant_elem;
        posiciones[i].ultima=aux-1;
    }

    tick = dwalltime();

    for(i=0;i<cant_threads;i++){
        pthread_create(&threads[i],&attr,mult_matrices,&posiciones[i]);
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
}