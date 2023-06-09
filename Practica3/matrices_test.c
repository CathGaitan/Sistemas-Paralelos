#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

/* Time in seconds from some point in the past */
double dwalltime();


int main(int argc,char*argv[]){
 double *A,*B,*C,*A2,*B2,*C2;
 int i,j,k,N;
 int check=1,check2=1;
 double timetick;

 //Controla los argumentos al programa
  if (argc < 3){
   printf("\n Faltan argumentos:: N dimension de la matriz, T cantidad de threads \n");
   return 0;
  }
  N=atoi(argv[1]);
  int numThreads=atoi(argv[2]);
  omp_set_num_threads(numThreads);
 
   
 //Aloca memoria para las matrices
   A=(double*)malloc(sizeof(double)*N*N);
   B=(double*)malloc(sizeof(double)*N*N);
   C=(double*)malloc(sizeof(double)*N*N);
   A2=(double*)malloc(sizeof(double)*N*N);
   B2=(double*)malloc(sizeof(double)*N*N);
   C2=(double*)malloc(sizeof(double)*N*N);

 //Inicializa las matrices A y B en 1, el resultado sera una matriz con todos sus valores en N
   for(i=0;i<N;i++){
      for(j=0;j<N;j++){
	      A[i*N+j]=1;
	      B[i+j*N]=1;
         A2[i*N+j]=1;
	      B2[i+j*N]=1;
      }
   }   

   timetick = dwalltime();
   //Paralelizando filas
   #pragma omp parallel for default(none) shared(A,B,C,N) private(i,j,k)
   for(i=0;i<N;i++){
      for(j=0;j<N;j++){
         C[i*N+j]=0;
         for(k=0;k<N;k++){
            C[i*N+j]= C[i*N+j] + A[i*N+k]*B[k+j*N];
         }
      }
   }   
  printf("Tiempo paralelizando filas %f \n", dwalltime() - timetick);

   timetick = dwalltime();
   //Paralelizando columnas
   for(i=0;i<N;i++){
      #pragma omp parallel for default(none) shared(A2,B2,C2,N) private(j,k) firstprivate(i)
      for(j=0;j<N;j++){
         C2[i*N+j]=0;
         for(k=0;k<N;k++){
            C2[i*N+j]= C2[i*N+j] + A2[i*N+k]*B2[k+j*N];
         }
      }
   }   
  printf("Tiempo paralelizando columnas %f \n", dwalltime() - timetick);

   //Verifica el resultado
   for(i=0;i<N;i++){
      for(j=0;j<N;j++){
	      check=check&&(C[i*N+j]==N);
      }
   }
   for(i=0;i<N;i++){
      for(j=0;j<N;j++){
	      check2=check2&&(C2[i*N+j]==N);
      }
  }

   if(check & check2){
      printf("Multiplicacion de matrices resultado correcto\n");
   }else{
      printf("Multiplicacion de matrices resultado erroneo\n");
  }

   free(A);
   free(B);
   free(C);
   return(0);
}



/*****************************************************************/

#include <sys/time.h>

double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}


