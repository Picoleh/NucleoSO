#include<NUCLEO.H>

PTR_BCP_PROC prim = NULL;
PTR_DESC d_esc;
APONTA_REG_CRIT rcDOS;

/* Coloca os valores iniciais no semaforo */
void far iniciaSemaforo(semaforo *sem, int v){
    sem->s = v;
    sem->Q = NULL;
}

/* Se o contador do semaforo é zero, bloqueia o processo corrente e o adiciona na lista de bloqueados.
    Caso contrario decrementa contador
*/
void far Down(semaforo *sem){
    PTR_BCP_PROC aux = prim;
    disable();
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

/* Desbloqueia o primeiro processo da lista, caso houver. Se não incrementa o contador */
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

/* Realiza uma insercao em uma lista encadeada simples */
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

/* Realiza uma insercao em uma lista encadeada circular */
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

/* Percorre a lista de BCP's, exibindo os nomes e estados dos processos */
void mostraFilaBCP(){
    PTR_BCP_PROC aux = prim;
    printf("FILA: \n");
    while(aux->prox_bcp != prim){
        printf("%s (%d) -> ", aux->nome, aux->estado);
        aux = aux->prox_bcp;
    }
    printf("%s (%d) -> l\n", aux->nome, aux->estado);
}

/* Aloca memoria para um novo BCP e insere valores inicias */
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

/* Percorre a lista de BCP's a procura de um processo ativo, retornando true ou false e 
   caso verdadeiro, altera o ponteiro prim do escalador 
*/
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

/* Implementa a logica do escalador, escolhendo o fluxo de execução e tratando interrupções em areas criticas do DOS */
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
            if(procuraProximoAtivo())
                p_est->p_destino = prim->contexto;
            else
                voltaDOS();
            enable();
        }
    }
}

/* Chama a corotina do escalador */
void far disparaSistema(){
    PTR_DESC d_dispara;

    d_esc = cria_desc();
    d_dispara = cria_desc();

    newprocess(escalador, d_esc);
    transfer(d_dispara, d_esc);
}

/* Muda o estado do processo para terminado */
void far terminaProcesso(){
    disable();
    prim->estado = terminado;
    enable();
    while(1);
}

/* Recupera o endereco da rotina de interrupção padrão do DOS, efetivamente saindo de execução */
void far voltaDOS(){
    disable();
    setvect(8, p_est->int_anterior);
    enable();
    exit(0);
}


