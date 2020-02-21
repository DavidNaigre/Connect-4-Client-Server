#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

#define clear() printf("\033[H\033[J")
char BLANK = ' ';
char pion;
char grille[6][7];
int GridAvancement, GridComplete = 42, nbr_col = 7, nbr_ligne = 6, upper = 7,lower = 0;

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
        if (grille[i][col_selected] == BLANK)
        {
            grille[i][col_selected] = pion;
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
int computerMove()
{
    return (rand() % (upper - lower + 1)) + lower;
}

// -------------------------- DIALOGUE CLIENT ------------------------
// Function designed for chat between client and server.
void server_play(int sockfd)
{
    char buff[MAX];
    int col, rep;
    for (;;)
    {
        read(sockfd, buff, sizeof(buff));
        col = atoi(buff);
        player_placement(col-1);
        rep = computerMove();
        printf("From client: %d\t To client : %d ...\n", col, rep);
        bzero(buff, MAX);
        sprintf(buff, "%d", rep);
        write(sockfd, buff, sizeof(buff));
        if (col == -10)
        {
            printf("Fermeture du serveur...\n");
            break;
        }
    }
}

// -------------------------- DIALOGUE CLIENT ------------------------
int main(int argc, char *argv[])
{
    srand(time(0));
    clear();
    init();
    int col, col_ordi, rep, sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("Échec de la création du socket...\n");
        exit(0);
    }
    else
        printf("Socket créé..\n");
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    if ((listen(sockfd, 5)) != 0)
    {
        printf("échec de mise en attente...\n");
        exit(0);
    }
    else
        printf("Serveur en attente..\n");
    len = sizeof(cli);

    connfd = accept(sockfd, (SA *)&cli, &len);
    if (connfd < 0)
    {
        printf("Échec de connexion...\n");
        exit(0);
    }
    else
        printf("Client connecté...\n");
    server_play(connfd);
    close(sockfd);
}
