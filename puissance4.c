#include <stdio.h>
#include <stdlib.h>

#define clear() printf("\033[H\033[J")
char BLANK  = ' ';
char pion;
char grille[6][7];
int GridAvancement, GridComplete = 42, nbr_col = 7, nbr_ligne = 6;

//FOnction d'initialisation de la grille du jeu
void init(){
  GridAvancement = 0;
  for (int i = 0; i < nbr_ligne; i++) {
    for (int j = 0; j < nbr_col; j++) {
      grille[i][j] = BLANK;
    }
  }
}

//Fonction d'assignation du numéro de joueur, avec serveur return CHAR placement en attente
char assignement(){
  return 'X';
}

//Fonction placement de touche
int player_placement(int col_selected, int turn){
  if(turn == 1) pion = 'X';
  else pion = 'O';
  for (int i = nbr_ligne - 1; i >= 0; i--) {
    if(grille[i][col_selected] == BLANK){
      grille[i][col_selected] = pion;
      GridAvancement++;
      return i;
    }
  }
  return -1;
}

int verif(){
  //Horizontal Check
  for ( int j = 0; j < nbr_col-3; j++ ) {
      for ( int i = 0; i < nbr_ligne; i++ ) {
          if(grille[i][j] == pion && grille[i][j+1] == pion && grille[i][j+2] == pion && grille[i][j+3] == pion) {
              return 1;
          }
      }
  }
  //VerticalCheck
  for ( int i = 0; i < nbr_ligne-3; i++ ) {
      for ( int j = 0; j < nbr_col; j++ ) {
          if(grille[i][j] == pion && grille[i+1][j] == pion && grille[i+2][j] == pion && grille[i+3][j] == pion) {
              return 1;
          }

      }
  }
  //ascendingDiagonalCheck
  for ( int i = 3; i < nbr_ligne; i++ ) {
      for ( int j = 0; j < nbr_col-3; j++ ) {
          if(grille[i][j] == pion && grille[i-1][j+1] == pion && grille[i-2][j+2] == pion && grille[i-3][j+3] == pion) {
              return 1;
          }
      }
  }
  //descendingDiagonalCheck
  for ( int i = 3; i < nbr_ligne; i++ ) {
      for ( int j = 3; j < nbr_col; j++ ) {
          if(grille[i][j] == pion && grille[i-1][j-1] == pion && grille[i-2][j-2] == pion && grille[i-3][j-3] == pion) {
              return 1;
          }
      }
  }
  return 0;
}

int GameRecovery(){
    return 1;
}

void affichage(){
  for (int i = 0; i < nbr_ligne; i++) {
    for (int j = 0; j < nbr_col; j++) {
      printf("%c |", grille[i][j]);
    }
    printf("\n");
  }
}

int main(int argc, char* argv[]){
  clear();
  init();
  int col, turn = 1, rep;
  pion = assignement();
  while(1){
    affichage();
    printf("\n\nveuillez choisir une colonne : ");
    scanf("%d",&col);
    rep = player_placement(col, turn);
    if(rep == -1){
      clear();
      printf("vous ne pouvez jouer là\n");
    }
    else{
      if(verif() == 1){
        clear();
        affichage();
        printf("Gagnant\n");
        return 0;
      }
      if(turn == 1) turn = 2;
      else turn = 1;
      clear();
    }
  }

}
