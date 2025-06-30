#include<system.h>
#include<stdio.h>

PTR_DESC descMain, descTic, descTac;

void far tac(){
 while(1){
  printf("tac ");
  transfer(descTac, descTic);
 }
 return;
}

void far tic(){
 while(1){
  printf("tic ");
  transfer(descTic, descTac);
 }
 return;
}

int main(){
 system("cls");
 descTic = cria_desc();
 descTac = cria_desc();
 descMain = cria_desc();

 newprocess(tic, descTic);
 newprocess(tac, descTac);

 transfer(descMain, descTic);
 return 0;
}