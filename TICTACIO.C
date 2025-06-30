#include<system.h>
#include<stdio.h>
#include<stdlib.h>

PTR_DESC dMain, dEscalador, dTic, dTac;

void far tac(){
  while(1){
    printf("tac ");
  }
  return;
}

void far tic(){
  while(1){
    printf("tic ");
  }
  return;
}

void far escalador(){
  int c = 5;
  int flag = 0;
  PTR_DESC dAtual;
  p_est->num_vetor = 8;

  while(c > 0){
    if(flag)
      dAtual = dTac;
    else
      dAtual = dTic;

    flag = !flag;

    p_est->p_origem = dEscalador;
    p_est->p_destino = dAtual;

    iotransfer();
    c -= 1;
  }

  printf("\nFim\n");
  while(1){
   transfer(dEscalador, dMain);
  }
}

int main(){
  system("cls");
  dMain = cria_desc();
  dEscalador = cria_desc();
  dTic = cria_desc();
  dTac = cria_desc();

  newprocess(escalador, dEscalador);
  newprocess(tic, dTic);
  newprocess(tac, dTac);

  system("pause");
  transfer(dMain, dEscalador);
  system("pause");
  return 0;
}