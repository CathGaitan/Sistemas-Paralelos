#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define COORDINATOR 0

static inline void mult_matrices(double *A, double *B, double *C, int stripSize, int tam_bloque, int N);
static inline void mult_bloques(double *ablk, double *bblk, double *cblk, int tam_bloque, int N);
static inline void sumar_matrices(double *AB, double *CD, double *EF, double *R, int stripSize, int N);


// R = AB + CD + EF
int main(int argc, char* argv[]){
	int i,j,k,cantProcesos,rank,N,stripSize,tam_bloque,check=1;
	double *A,*B,*C,*D,*E,*F,*AB,*CD,*EF,*R;
	MPI_Status status;
	double commTimes[4], maxCommTimes[4], minCommTimes[4], commTime, totalTime;

	/* Lee parametros de la linea de comando */
	if ((argc != 3) || ((N = atoi(argv[1])) <= 0) ) {
	    printf("Param1: tamanio matriz - Param2: tamanio bloque: \n");
		exit(1);
	}
	tam_bloque=atoi(argv[2]);

	MPI_Init(&argc,&argv);

	MPI_Comm_size(MPI_COMM_WORLD,&cantProcesos);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	if (N % cantProcesos != 0) {
		printf("El tamanio de la matriz debe ser multiplo del numero de procesos.\n");
		exit(1);
	}

    //Calcular porcion de cada worker
	stripSize = N / cantProcesos;

    // Reservar memoria
	if (rank == COORDINATOR) {
		A = (double*) malloc(sizeof(double)*N*N);
		C = (double*) malloc(sizeof(double)*N*N);
        E = (double*) malloc(sizeof(double)*N*N);
        R = (double*) malloc(sizeof(double)*N*N);
	}
	else  {
		A = (double*) malloc(sizeof(double)*N*stripSize);
		C = (double*) malloc(sizeof(double)*N*stripSize);
        E = (double*) malloc(sizeof(double)*N*stripSize);
        R = (double*) malloc(sizeof(double)*N*stripSize);
	}
	
	//todos los procesos necesitan todas las columnas
	B = (double*) malloc(sizeof(double)*N*N);
    D = (double*) malloc(sizeof(double)*N*N);
    F = (double*) malloc(sizeof(double)*N*N);

	//matrices intermedias
	AB = (double*) malloc(sizeof(double)*N*N);
    CD = (double*) malloc(sizeof(double)*N*N);
    EF = (double*) malloc(sizeof(double)*N*N);

    //Inicializar datos
	if(rank == COORDINATOR){
	    for(i=0;i<N;i++){
            for (j=0;j<N;j++){
				A[i*N+j]=1;
                C[i*N+j]=1;
                E[i*N+j]=1;
                B[j*N+i]=1;
                D[j*N+i]=1;
                F[j*N+i]=1;
            }
        }
	}
    
    //Espero a que el coordinador inicialice
    MPI_Barrier(MPI_COMM_WORLD);

	commTimes[0] = MPI_Wtime();

    //Distribuyo los datos
	MPI_Scatter(A,stripSize*N,MPI_DOUBLE,A,stripSize*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);
    MPI_Scatter(C,stripSize*N,MPI_DOUBLE,C,stripSize*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);
    MPI_Scatter(E,stripSize*N,MPI_DOUBLE,E,stripSize*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);

	MPI_Bcast(B,N*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);
    MPI_Bcast(D,N*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);
	MPI_Bcast(F,N*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);

    commTimes[1] = MPI_Wtime();

    //Hacer mult matrices -> cada proceso hace su partecita
	// R = AB + CD + EF
	mult_matrices(A,B,AB,stripSize,tam_bloque,N);
	mult_matrices(C,D,CD,stripSize,tam_bloque,N);
	mult_matrices(E,F,EF,stripSize,tam_bloque,N);
	sumar_matrices(AB,CD,EF,R,stripSize,N);

	commTimes[2] = MPI_Wtime();
	
	//Juntar todos los calculos 
	MPI_Gather(R,stripSize*N,MPI_DOUBLE,R,stripSize*N,MPI_DOUBLE,COORDINATOR,MPI_COMM_WORLD);
	
	commTimes[3] = MPI_Wtime();

	MPI_Reduce(commTimes, minCommTimes, 4, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
	MPI_Reduce(commTimes, maxCommTimes, 4, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);

	MPI_Finalize();
	if (rank==COORDINATOR) {
		for(i=0;i<N;i++){
			for(j=0;j<N;j++){
				check=check&&(R[i*N+j]==3*N);
			}
		}
	if(check){
		printf("Multiplicacion de matrices resultado correcto\n");
	}else{
		printf("Multiplicacion de matrices resultado erroneo\n");
	}
	
	totalTime = maxCommTimes[3] - minCommTimes[0];
	commTime = (maxCommTimes[1] - minCommTimes[0]) + (maxCommTimes[3] - minCommTimes[2]);
	printf("Multiplicacion de matrices con N=%d, tam_bloque=%d, cant_procesos=%d\n",N,tam_bloque,cantProcesos);
	printf("Tiempo total=%lf, Tiempo comunicacion=%lf, Tiempo computo=%lf",totalTime,commTime,totalTime-commTime);
	}

	free(A);
	free(B);
	free(C);
	free(D);
	free(E);
	free(F);
	free(R);
	free(AB);
	free(CD);
	free(EF);
	return 0;
}

static inline void mult_matrices(double *A, double *B, double *C, int stripSize,int tam_bloque, int N){
    int i,j,k;
	for (i=0;i<stripSize;i+=tam_bloque){
		int valori=i*N;
		for (j=0;j<N;j+=tam_bloque){
		    int valorj=j*N;
			for (k=0;k<N;k+=tam_bloque) { 
                mult_bloques(&A[valori+k], &B[valorj+k], &C[valori+j],tam_bloque,N);
			}
		}
	}
}

static inline void mult_bloques(double *ablk, double *bblk, double *cblk, int tam_bloque, int N){
    int i,j,k; 
    for(i=0;i<tam_bloque;i++){
        int valori=i*N;
        for(j=0;j<tam_bloque;j++){
            double temp=0;
            int valorj=j*N;
            for(k=0;k<tam_bloque;k++){
                temp+=ablk[valori+k]*bblk[valorj+k];
            }
            cblk[valori+j]+=temp;
        }
    }
}

static inline void sumar_matrices(double *AB, double *CD, double *EF, double *R, int stripSize, int N){
	int i;
	for(i=0;i<stripSize*N;i++){
		R[i]=AB[i]+CD[i]+EF[i];
	}
}
