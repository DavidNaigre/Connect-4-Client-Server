#include <stdio.h>
#include <stdlib.h>

char BLANK  = ' ';
char pion[2] = {'X','O'};
char grille[6][7];
int GridAvancement, GridComplete = 42, nbr_col = 7, nbr_ligne = 6;

void init(){
  GridAvancement = 0;
  for (int i = 0; i < nbr_ligne; i++) {
    for (int j = 0; j < nbr_col; j++) {
      grille[i][j] = 'T';
    }
  }
}



int main(int argc, char* argv[]){
  init();
    for (int i = 0; i < nbr_ligne; i++) {
      for (int j = 0; j < nbr_col; j++) {
        printf("%c", grille[i][j]);
      }
      printf("\n");
    }
}
