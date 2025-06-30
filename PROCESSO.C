#include<NUCLEO.H>
#include<stdio.h>

semaforo opa;

void far processo1(){
    int i=0;
    while(i < 30000){
        printf("1");
        i++;
        Down(&opa);
    }
    terminaProcesso();
}

void far processo2(){
    int i=0;
    while(i < 10000){
        printf("2");
        i++;
    }
    terminaProcesso();
}

int main(){
    criaProcesso("proc1", processo1);
    criaProcesso("proc2", processo2);
    iniciaSemaforo(&opa, 20000);
    disparaSistema();
}