#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>

bool esPotencia(int n){
    return 0 == (n & n-1);
}

int main(int argc, char *argv[]){
    int cantArgs=argc-1;
    float *numeros;
    int k,j,i,posDivisor=1,sumarI=2;
    float cantIter=log2(cantArgs);
    
    numeros=(float*)malloc(sizeof(float)*cantArgs);

    if(!esPotencia(cantArgs)){ //le resto uno porque siempre hay un parametro
        printf("La cantidad de parametros debe ser potencia de dos");
        exit(1);
    }

    for(k=0;k<cantArgs;k++){ //parseo los strings a float
        numeros[k]=atoi(argv[k+1]);
    }


    for(j=0;j<cantIter;j++){ //hago la reduccion
        i=0;
        while((i<cantArgs)&&(i+posDivisor != cantArgs)){ 
            float result = numeros[i]/numeros[i+posDivisor];
            printf("%f / %f = %f\n",numeros[i],numeros[i+posDivisor],result);
            numeros[i]=result;
            i+=sumarI;

        }
        printf("----\n");
        posDivisor=posDivisor*2;
        sumarI=sumarI*2;
    }
    printf("Resultado: %f\n",numeros[0]);

    free(numeros);
    return(0);
}
