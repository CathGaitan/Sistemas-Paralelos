#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

bool esPotencia(int n){
    return 0 == (n & n-1);
}

int main(int argc, char *argv[]){
    int cantArgs=argc-1;
    float *numeros;
    int k,j,i,pos;
    
    numeros=(float*)malloc(sizeof(float)*cantArgs);

    if(!esPotencia(cantArgs)){ //le resto uno porque siempre hay un parametro
        printf("La cantidad de parametros debe ser potencia de dos");
        exit(1);
    }

    for(k=0;k<cantArgs;k++){ //parseo los strings a float
        numeros[k]=atoi(argv[k+1]);
    }

    for(i=cantArgs-1;i>0;i=i/2){ //hago la reduccion
        pos=0;
        for(j=0;j<i;j=j+2){
            float result = numeros[j]/numeros[j+1];
            printf("%f / %f = %f\n",numeros[j],numeros[j+1],result);
            numeros[pos]=result;
            pos++;
        }
        printf("----\n");  
    }
    printf("Resultado: %f\n",numeros[0]);
    free(numeros);
    return(0);
}
