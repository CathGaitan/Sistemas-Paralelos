#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mpi.h" 

#define MASTER 0

double dwalltime();

int main(int argc, char* argv[]){
    double tick;
    int myrank;
    int size;
    int dest;              /* destination rank for message */
    int source;            /* source rank of a message */
    int tag = 0;           /* scope for adding extra information to a message */
    MPI_Status status;     /* struct used by MPI_Recv */
    char message[BUFSIZ];

    MPI_Init(&argc, &argv );

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank); 

    tick = dwalltime();

    if(myrank == 0){
        sprintf(message, "Hola Mundo! Soy el proceso %d!", myrank);
        MPI_Send(message,strlen(message)+1,MPI_CHAR,myrank+1,tag,MPI_COMM_WORLD);
        MPI_Recv(message, BUFSIZ, MPI_CHAR,size,tag, MPI_COMM_WORLD, &status);
        printf("Mensaje recibido: %s\n", message);
    }

    if(myrank == size){
        sprintf(message, "Hola Mundo! Soy el proceso %d!", myrank);
        MPI_Recv(message, BUFSIZ, MPI_CHAR,myrank-1, tag, MPI_COMM_WORLD, &status);
        MPI_Send(message,strlen(message)+1,MPI_CHAR,0,tag,MPI_COMM_WORLD);
        printf("Mensaje recibido: %s\n", message);  
    }

    if(myrank%2 == 0 && myrank !=size && myrank != 0){ //si mi rank es par
        sprintf(message, "Hola Mundo! Soy el proceso %d!", myrank);
        MPI_Send(message,strlen(message)+1,MPI_CHAR,myrank+1,tag,MPI_COMM_WORLD);
        MPI_Recv(message, BUFSIZ, MPI_CHAR,myrank-1, tag, MPI_COMM_WORLD, &status);
        printf("Mensaje recibido: %s\n", message);
    }else{
        sprintf(message, "Hola Mundo! Soy el proceso %d!", myrank);
        MPI_Recv(message, BUFSIZ, MPI_CHAR,myrank-1, tag, MPI_COMM_WORLD, &status);
        MPI_Send(message,strlen(message)+1,MPI_CHAR,myrank+1,tag,MPI_COMM_WORLD);
        printf("Mensaje recibido: %s\n", message);    
    }
    MPI_Finalize();

    printf("Tiempo requerido con %i procesos: %f\n",size,dwalltime()-tick);

    return EXIT_SUCCESS;
}

double dwalltime(){
	double sec;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}