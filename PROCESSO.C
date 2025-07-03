#include<NUCLEO.H>
#include<stdio.h>
#include <stdlib.h>

#define N 100
#define IT 1000
semaforo mutex, vazia, cheia;
int inicio, fim, vetor[N];
FILE *arq;

/* função produtora, onde é retornado um numero ímpar a partir do valor de entrada seed */
int produzir(int seed){
    return 2 * seed + 1;
}

/* Produz IT produtos e deposita no buffer, quando disponivel */
void far produtor(){
    int i=0;
    while(i < IT){
        int prod = produzir(i);
        Down(&vazia);
        Down(&mutex);
        vetor[inicio] = prod;
        fprintf(arq, "produtor depositou em [%d]: %d\n", inicio++, prod);
        inicio = inicio % N;
        Up(&mutex);
        Up(&cheia);
        i++;
    }
    terminaProcesso();
}

/* Consome IT produtos do buffer, quando disponivel*/
void far consumidor(){
    int i=0;
    while(i < IT){
        int consumido = vetor[fim];
        Down(&cheia);
        Down(&mutex);
        fprintf(arq, "consumidor retirou em [%d]: %d\n", fim++, consumido);
        fim = fim % N;
        Up(&mutex);
        Up(&vazia);
        i++;
    }
    terminaProcesso();
}

int main(){
    arq = fopen("LOG.TXT", "a");

    if(arq == NULL){
        printf("Erro ao abrir arquivo\n");
        exit(1);
    }

    iniciaSemaforo(&mutex, 1);
    iniciaSemaforo(&vazia, N);
    iniciaSemaforo(&cheia, 0);
    inicio = 0;
    fim = 0;
    criaProcesso("prod", produtor);
    criaProcesso("cons", consumidor);
    disparaSistema();
    fclose(arq);
}