#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define COORDINATOR 0

/* Desarrolle un algoritmo paralelo que dado dos vectores (V y V2) de 
   tamaño N obtenga el valor máximo, el valor mínimo y valor promedio 
   de sus elementos. */
   
int main(int argc, char* argv[]){
	int i,cantProcesos,rank,N,stripSize,check=1;
	double *V,*V2;
	MPI_Status status;
	double commTimes[4], maxCommTimes[4], minCommTimes[4], commTime, totalTime;

   if ((argc != 2) || ((N = atoi(argv[1])) <= 0)) {
	   printf("Param1: tamanio vector \n");
		exit(1);
	}
   
   MPI_Init(&argc,&argv);

	MPI_Comm_size(MPI_COMM_WORLD,&cantProcesos);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	if (N % cantProcesos != 0) {
		printf("El tamanio del vector debe ser multiplo del numero de procesos.\n");
		exit(1);
	}

   //Calcular porcion de cada worker
	stripSize = N / cantProcesos;

   // Reservar memoria
	if (rank == COORDINATOR) {
	   V = (double*) malloc(sizeof(double)*N);
      V2 = (double*) malloc(sizeof(double)*N);
	}else{
		V = (double*) malloc(sizeof(double)*stripSize);
      V2 = (double*) malloc(sizeof(double)*stripSize);
	}

   if(rank == COORDINATOR){
	   for(i=0;i<N;i++){
			V[i]=rand()%100+1; 
         V2[i]=rand()%999+1;  
      }
	}

   //Espero a que el coordinador inicialice
   MPI_Barrier(MPI_COMM_WORLD);

	commTimes[0] = MPI_Wtime();

   //Distribuyo los vectores
	MPI_Scatter(V,stripSize,MPI_DOUBLE,V,stripSize,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);
	MPI_Scatter(V2,stripSize,MPI_DOUBLE,V2,stripSize,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);


   commTimes[1] = MPI_Wtime();

   //calcular valores
   //Pos 0 -> min, max y suma de V
   //Pos 1 -> min, max y suma de V2
   double mins[2],maxs[2],sumas[2];
   double minsR[2],maxsR[2],sumasR[2];
   mins[0]=V[0]; maxs[0]=V[0]; sumas[0]=0;
   mins[1]=V2[0]; maxs[1]=V2[0]; sumas[1]=0;

   for(i=0;i<stripSize;i++){
      if(V[i] > maxs[0]) 
            maxs[0] = V[i];
        if(V[i] < mins[0]) 
            mins[0] = V[i];
        sumas[0] += V[i];
   }
   for(i=0;i<stripSize;i++){
      if(V2[i] > maxs[1]) 
            maxs[1] = V2[i];
        if(V2[i] < mins[1]) 
            mins[1] = V2[i];
        sumas[1] += V2[i];
   }

   commTimes[2] = MPI_Wtime();

   MPI_Allreduce(mins,minsR,2,MPI_DOUBLE,MPI_MIN,MPI_COMM_WORLD);
   MPI_Allreduce(maxs,maxsR,2,MPI_DOUBLE,MPI_MAX,MPI_COMM_WORLD);
   MPI_Allreduce(sumas,sumasR,2,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD);

   commTimes[3] = MPI_Wtime();

   MPI_Reduce(commTimes, minCommTimes, 4, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
	MPI_Reduce(commTimes, maxCommTimes, 4, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);

   MPI_Finalize();
   if(rank==COORDINATOR){
      printf("Elementos V: ");
      for(i=0;i<N;i++){
         printf("%f, ",V[i]);
      }
      printf("\n");
      printf("Elementos V2: ");
      for(i=0;i<N;i++){
         printf("%f, ",V2[i]);
      }
      printf("\n");
      printf("minV=%f, maxV=%f, sumaV=%f \n",minsR[0],maxsR[0],sumasR[0]);
      printf("minV2=%f, maxV2=%f, sumaV2=%f \n",minsR[1],maxsR[1],sumasR[1]);
      totalTime = maxCommTimes[3] - minCommTimes[0];
	   commTime = (maxCommTimes[1] - minCommTimes[0]) + (maxCommTimes[3] - minCommTimes[2]);
	   printf("Tiempo total=%lf, Tiempo comunicacion=%lf, Tiempo computo=%lf\n",totalTime,commTime,totalTime-commTime);
   }

   free(V);
   free(V2);
   return 0;
}