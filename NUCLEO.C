#include<NUCLEO.H>
/*typedef struct desc_p{
    char nome[35];
    enum {ativo, bloqueado, terminado} estado;
    PTR_DESC contexto;
    struct desc_p *prox_bcp;
} BCP_PROC;

typedef BCP_PROC* PTR_BCP_PROC;*/

PTR_BCP_PROC prim = NULL;
PTR_DESC d_esc;
APONTA_REG_CRIT rcDOS;

void far iniciaSemaforo(semaforo *sem, int v){
    sem->s = v;
    sem->Q = NULL;
}

void far Down(semaforo *sem){
    PTR_BCP_PROC aux = prim;
    disable();
    system("pause");
    if(sem->s == 0){
        insereFimFilaSemaforo(sem, prim);
        prim->estado = bloqueado;

        if(procuraProximoAtivo())
            transfer(aux->contexto, prim->contexto);
        else
            voltaDOS();
    }
    else{
        sem->s--;
        enable();
    }
}

void far Up(semaforo *sem){
    disable();
    if(sem->Q != NULL){
        sem->Q->estado = ativo;
        sem->Q = sem->Q->fila_sem;
    }
    else
        sem->s++;
    enable();
}

void insereFimFilaSemaforo(semaforo *sem, PTR_BCP_PROC bcp){
    if(sem->Q == NULL)
        sem->Q = bcp;
    else{
        PTR_BCP_PROC aux = sem->Q;
        while (aux->fila_sem != NULL)
            aux = aux->fila_sem;
        aux->fila_sem = bcp;
    }
}

void insereFimFilaBCP(PTR_BCP_PROC bcp){
    if(prim == NULL)
        prim = bcp;
    else{
        PTR_BCP_PROC aux = prim;
        while (aux->prox_bcp != prim)
            aux = aux->prox_bcp;
        aux->prox_bcp = bcp;
    }
    bcp->prox_bcp = prim;
}

void mostraFilaBCP(){
    PTR_BCP_PROC aux = prim;
    printf("FILA: \n");
    while(aux->prox_bcp != prim){
        printf("%s (%d) -> ", aux->nome, aux->estado);
        aux = aux->prox_bcp;
    }
    printf("%s (%d) -> l\n", aux->nome, aux->estado);
}

void far criaProcesso(char* nome_p, void far(*end_proc)()){
    PTR_BCP_PROC bcp = (PTR_BCP_PROC) malloc(sizeof(BCP_PROC));

    if(bcp == NULL){
        printf("Falha na alocacao de memoria\n");
        return;
    }

    strcpy(bcp->nome, nome_p);

    bcp->estado = ativo;

    bcp->contexto = cria_desc();

    bcp->fila_sem = NULL;

    newprocess(end_proc, bcp->contexto);

    insereFimFilaBCP(bcp);
}

int procuraProximoAtivo(){
    PTR_BCP_PROC aux = prim->prox_bcp;
    while(aux != prim){
        if(aux->estado == ativo){
            prim = aux;
            return 1;
        }
        aux = aux->prox_bcp;
    }

    if(prim->estado == ativo)
        return 1;
    else
        return 0;
}

void far escalador(){
    p_est->p_origem = d_esc;
    p_est->p_destino = prim->contexto;
    p_est->num_vetor = 8;

    _AH = 0x34;
    _AL = 0x00;
    geninterrupt(0x21);
    rcDOS.x.bx1 = _BX;
    rcDOS.x.es1 = _ES;

    while(1){
        iotransfer();
        disable();
        if(!*rcDOS.y){
            printf("\nprocesso %s rodou\n", prim->nome);
            if(procuraProximoAtivo())
                p_est->p_destino = prim->contexto;
            else
                voltaDOS();
            enable();
        }
    }
}

void far disparaSistema(){
    PTR_DESC d_dispara;

    mostraFilaBCP();
    system("pause");

    d_esc = cria_desc();
    d_dispara = cria_desc();

    newprocess(escalador, d_esc);
    transfer(d_dispara, d_esc);
}

void far terminaProcesso(){
    disable();
    prim->estado = terminado;
    printf("%s terminou\n", prim->nome);
    enable();
    while(1);
}

void far voltaDOS(){
    disable();
    printf("Voltando ao DOS\n");
    mostraFilaBCP();
    setvect(8, p_est->int_anterior);
    system("pause");
    enable();
    exit(0);
}


