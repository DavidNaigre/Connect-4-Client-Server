#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

#define clear() printf("\033[H\033[J")
char BLANK = ' ';
char pion;
char grille[6][7];
int GridAvancement, GridComplete = 42, nbr_col = 7, nbr_ligne = 6;

//FOnction d'initialisation de la grille du jeu
void init()
{
  GridAvancement = 0;
  for (int i = 0; i < nbr_ligne; i++)
  {
    for (int j = 0; j < nbr_col; j++)
    {
      grille[i][j] = BLANK;
    }
  }
}
//Fonction placement de touche
int player_placement(int col_selected)
{
  pion = 'X';
  for (int i = nbr_ligne - 1; i >= 0; i--)
  {
    if (grille[i][col_selected - 1] == BLANK)
    {
      grille[i][col_selected - 1] = pion;
      GridAvancement++;
      return i;
    }
  }
  return -1;
}
//Fonction placement de l'ordinateur
int ordi_placement(int col_selected)
{
  pion = 'O';
  for (int i = nbr_ligne - 1; i >= 0; i--)
  {
    if (grille[i][col_selected] == BLANK)
    {
      grille[i][col_selected] = pion;
      GridAvancement++;
      return i;
    }
  }
  return -1;
}

int verif()
{
  //Horizontal Check
  for (int j = 0; j < nbr_col - 3; j++)
  {
    for (int i = 0; i < nbr_ligne; i++)
    {
      if (grille[i][j] == pion && grille[i][j + 1] == pion && grille[i][j + 2] == pion && grille[i][j + 3] == pion)
      {
        return 1;
      }
    }
  }
  //VerticalCheck
  for (int i = 0; i < nbr_ligne - 3; i++)
  {
    for (int j = 0; j < nbr_col; j++)
    {
      if (grille[i][j] == pion && grille[i + 1][j] == pion && grille[i + 2][j] == pion && grille[i + 3][j] == pion)
      {
        return 1;
      }
    }
  }
  //ascendingDiagonalCheck
  for (int i = 3; i < nbr_ligne; i++)
  {
    for (int j = 0; j < nbr_col - 3; j++)
    {
      if (grille[i][j] == pion && grille[i - 1][j + 1] == pion && grille[i - 2][j + 2] == pion && grille[i - 3][j + 3] == pion)
      {
        return 1;
      }
    }
  }
  //descendingDiagonalCheck
  for (int i = 3; i < nbr_ligne; i++)
  {
    for (int j = 3; j < nbr_col; j++)
    {
      if (grille[i][j] == pion && grille[i - 1][j - 1] == pion && grille[i - 2][j - 2] == pion && grille[i - 3][j - 3] == pion)
      {
        return 1;
      }
    }
  }
  return 0;
}

void affichage()
{
  for (int i = 0; i < nbr_ligne; i++)
  {
    for (int j = 0; j < nbr_col; j++)
    {
      printf("%c |", grille[i][j]);
    }
    printf("\n");
  }
}

int Checkwniner(int joueur)
{
  if (verif() == 1)
  {
    clear();
    affichage();
    printf("--------------------");
    printf("\n 1| 2| 3| 4| 5| 6| 7|");
    if (joueur == 1) printf("\n\nL'humanoïde a gagné\n");
    else printf("\n\nLe serveur a gagner\n");
    return 1;
  }
  else
    0;
}
// -------------------------- DIALOGUE SERVEUR -----------------------
int server_play(int sockfd, int col)
{
  int n, rep_serv;
  char buff[MAX];
  sprintf(buff, "%d", col);
  write(sockfd, buff, sizeof(buff));
  bzero(buff, sizeof(buff));
  read(sockfd, buff, sizeof(buff));
  rep_serv = atoi(buff);
  return rep_serv;
}
// -------------------------- DIALOGUE SERVEUR ------------------------
int main(int argc, char *argv[])
{
  clear();
  init();
  int col, col_ordi, rep;

  // ~~~~~~~~~~~~~~~~~~~~~~~SERVEUR INITIALISATION DÉBUT
  int sockfd, connfd;
  struct sockaddr_in servaddr, cli;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1)
  {
    printf("Échec de la création du socket...\n");
    exit(0);
  }
  else
    printf("Socket créé...\n");
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(PORT);
  if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
  {
    printf("Impossible de joindre le serveur...\n");
    exit(0);
  }
  else
    printf("La partie peux commencer...\n");
  // ~~~~~~~~~~~~~~~~~~~~~~~SERVEUR INITIALISATION FIN

  while (1)
  {
    affichage();
    printf("--------------------");
    printf("\n 1| 2| 3| 4| 5| 6| 7|");
    printf("\n\nveuillez choisir une colonne : ");
    scanf("%d", &col);
    rep = player_placement(col);
    if (rep == -1)
    {
      clear();
      printf("vous ne pouvez jouer là\n");
    }
    else
    {
      if (Checkwniner(1) == 1)
        break;
      col_ordi = server_play(sockfd, col);
      ordi_placement(col_ordi);
      if (Checkwniner(2) == 1)
        break;
      clear();
      printf("\nL'ordinateur joue dans la colonne : %d\n\n", (col_ordi + 1));
    }
  }
  // function for chat
  // close the socket
  close(sockfd);
}
