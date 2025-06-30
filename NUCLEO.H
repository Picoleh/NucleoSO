#include<stdio.h>
#include <dos.h> far
#include<system.h>
#include<stdlib.h>
#include<string.h>

typedef struct reg{
    unsigned bx1, es1;
}registradores;

typedef union k{
    registradores x;
    char far *y;
}APONTA_REG_CRIT;


typedef struct desc_p{
    char nome[35];
    enum {ativo, bloqueado, terminado} estado;  
    PTR_DESC contexto;
    struct desc_p *fila_sem;
    struct desc_p *prox_bcp;
} BCP_PROC;

typedef BCP_PROC* PTR_BCP_PROC;

typedef struct sema{
    int s;
    PTR_BCP_PROC Q;
} semaforo;

extern PTR_BCP_PROC prim;
extern PTR_DESC d_esc;
extern APONTA_REG_CRIT rcDOS;

extern void far iniciaSemaforo(semaforo *sem, int v);

extern void far Down(semaforo *sem);

extern void far Up(semaforo *sem);

void insereFimFilaSemaforo(semaforo *sem, PTR_BCP_PROC bcp);

void insereFimFilaBCP(PTR_BCP_PROC bcp);

void mostraFilaBCP();

extern void far criaProcesso(char* nome_p, void far(*end_proc)());

int procuraProximoAtivo();

void far escalador();

extern void far disparaSistema();

extern void far terminaProcesso();

extern void far voltaDOS();