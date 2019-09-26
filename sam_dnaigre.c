#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#include<errno.h>
#include<time.h>
#include<readline/readline.h>
#include<readline/history.h>

#define clear() printf("\033[H\033[J")

#define MAX_INPUT_SZ 300
#define MAXCMD 20
#define MAXPARAM 50


void start_shell()
{
    clear();

    char* username = getenv("USER");

	printf("\n\n\n\n\t  \033[31;1m,-~~-.___.\033[0m");
	printf("\n\t \033[31;1m/ | \033[0m '\033[31;1m     \\\033[0m");
	printf("\n\t\033[31;1m(  )         0\033[0m");
	printf("\n\t\033[31;1m \\_/-, ,----'\033[0m ");
	printf("\n\t    \033[30;43mSÀM\033[0m           //  ");
	printf("\n\t \033[31;1m  /  \\-'~;\033[0m    \033[34;1m/~~~(O)\033[0m\t\tBIENVENUE DANS  SÀM Shell");
	printf("\n\t\033[31;1m  /  __/~|\033[0m   \033[34;1m/       |\033[0m");
	printf("\n\t\033[31;1m=(  _____|\033[0m \033[34;1m(_________|\033[0m\t\t*****By David NAIGRE*****");
    printf("\n\n\nUtilisateur actuel: %s\n", username);
  
    sleep(2);
    clear();
}
//Fonction servant à remplacer le saut à la ligne, en fin de commande, par une signal de fin de tableau. 
//Puis décomposer cette saisie en plusieur partie à chaque fois que strsep est confronté à un espace, afin d'être exécuter avec execvp.
void split_input(char* saisie, char** cmd)
{

	int taille, i, leave = 0;
	taille = strlen(saisie);

	if(saisie[strlen(saisie)-1] == '\n' )
	{
           saisie[strlen(saisie)-1] = '\0';
    }
    else if ((saisie[strlen(saisie)-1] == '\n' && saisie[strlen(saisie)-2] == '&'))
    {
    	saisie[strlen(saisie)-1] = '\0';
    	saisie[strlen(saisie)-2] = '\0';
    }

	for(i = 0; i < taille && leave == 0; i++)
	{
		cmd[i] = strsep(&saisie," ");				
		if(cmd[i] == NULL)
		{
			leave = 1;
		}
	}
}
//Fonction servant a afficher l'historique via la commande "history" quand le programme est lancé
int historique(char* backup[], int u)
{
    int i = u;
    int place = 1;
    do
    {
        if (backup[i])
        {
 		    printf("%4d:  %s\n", place, backup[i]);
            place++;
        }
        i = (i + 1) % MAXCMD;
        } while (i != u);

        return 0;	
}

//Fonction d'exécution de la commande issue de la découpe de la saisie utilisateur.
//Elle permet et est capable tout d'abord d'exécuter la commande utilisateur, 
//puis affiche un message informant l'utilisateur de la terminaison (succès, erreur) des commandes lancées aussi bien en premier plan, qu'en arrière plan.
int execution(char** cmd, int b, int pass)
{
	int  status;
	char* erreur;
	char* err;
	pid_t pid;
	if( b == 0)
	{
		pid = fork();
		if (pid < 0 )
		{
			printf("\nErreur de creation du processus\n");
		}

		else 
		{
			if (pid == 0)
			{
				exit(execvp(cmd[0], cmd));			//On exit execvp, pour pouvoir gérer les messages de terminaison et le passage en arrière plan ou non de la commande exécuter.
			}
			if (pass == 0)
			{
				waitpid(pid,&status,0);
				if(status == 0)
				{
					printf("\nProcessus executer sans echec\n");
				}
				else if (status != 0)
				{
					printf("La commande n'a pas pu être executer\n");
					strerror(errno);
					return 0;
				}   	
			}
			else if (pass == 1)					//Quand pass = 1, celà veut dire que la fonction background_verif à bien détecté un '&', et ainsi après le fork() le père n'attendra pas son fils.
			{ 									//ce qui créera un processus en arrière plan, avec un shell non bloqué.
				printf("PID: [%d] ",pid);
				pid = waitpid(-1,NULL,WNOHANG);
				strerror(errno);
				if (pid > 0)
				{
					printf("\nLe processus ayant le PID: %d à été tué\n",pid);
				}
			}
			
		}
	}
}
//Fonction servant a exécuter les commandes personnaliser et dites built-in non gérer par "execvp".
int* switchcmd(char** cmd,char** backup, int *b,int u, char holdchdir[100][10],int *z,int ret[])
{
	int i, n = 7, switch_value = 0;
    char* error;
	char* listcmd[n];
	char* username = getenv("USER");
	char cwd[1024];
	char* rec;
	rec = malloc(100* sizeof(char));

	listcmd[0] = "cd";
	listcmd[1] = "exit";
	listcmd[2] = "history";
	listcmd[3] = "help";
	listcmd[4] = "pushd";
	listcmd[5] = "popd";
	listcmd[6] = "debpush";

	for (i = 0; i < n; ++i)
	{
		if(strcmp(cmd[0], listcmd[i]) == 0) 		//comparaison entre la première partie issue de la décomposition de la commande de l'utilisateur avec tout le tableau de commande personnaliser.
		{											//en cas de match, la "recherche " est stopé, tout en renvoyant une valeur de sélection du cas à exécuter dans le switch  
			switch_value = i + 1;
			*b = 1;
			break;
		}
	}

	switch (switch_value)
	{
		case 1:
			chdir(cmd[1]);
        	break;

        case 2:

       		printf("\t\t\t\t    \033[1mMerci d'avoir utilisé SÀM Shell !\033[0m\n"
       			"\nDéveloppé en Avril 2018 dans le cadre d'un projet par David NAIGRE, Étudiant en L2 Informatique à l'Université des Antilles.\n"
       			"\n\t\t\t\t\tdnaigre.univ@gmail.com\n\n");
			exit(0);

		case 3:
			historique((char**)backup,u);
			break;

		case 4:
			printf("\n\t\033[33;1m*** %s, un soucis ? Pas de soucis, voici le manuel d'utilisation: ***\033[0m\n"
				"\nVous pouvez utiliser les flèches directionnelles 'haut' et 'bas' pour accéder à vos commandes précédentes."
				"\nSi vous souhaitez ouvrir un programme en arrière plan, rajoutez ' & ' à la fin de votre ligne de commande."
				"\n\nEn plus de quelques commandes personnalisées, toutes les commandes du Shell basique sont permises et fontionnelles !"
				"\n\n  cd <répertoire>    \tPour changer de répertoire courant"
				"\n\t\t\tajouter le répertoire voulu en parametre (exemple: .. , -)"
				"\n\n  history\t\tPour afficher, sous forme de liste, au maximum vos 20 dernières commandes saisies"
				"\n\n  help   \t\tPour afficher le manuel d'utilisation"
				"\n\n  exit   \t\tPour quitter SÀM Shell\n",username);
			break;
		
		case 5:

			rec = getcwd(cwd, sizeof(cwd));
			chdir(cmd[1]);
			error = strerror(errno);
			if (*error != 0)
        	{
        		printf("sam: %s: %s\n", cmd[0], error);
        	}
			if (*errr == 0)
			{
				sprintf(holdchdir[*z],"%s",rec);
				printf("Chemin stocké: %s\n",holdchdir[*z]);
				(*z) = (*z) + 1;
			}
			printf("z value in pushd: %d\n", *z);
			
			break;
		
		case 6:

			if ( ((*z) - 1) >= 0)
			{
				chdir(holdchdir[(*z)-1]);
				(*z) = (*z) - 1;
			}
			else
			{
				printf("Aucun chemin\n");
			}
			printf("z value in popd: %d\n", *z);
			
			break;

		case 7:
			for(i = 0; i < (*z); i ++)
			{
				printf("%s \033[34;1m|| \033[0m",holdchdir[i]);
			}
	}

	return holdchdir;
	//sprintf(ret,"%d %d %s",b,z,holdchdir);					//b est une variable servant bloquer l'exécution des commandes dans la fonction "execution".
								//Si 1, execution sera bloqué, si 0, la commande de l'utilisateur sera bien lancer dans la fonction d' execution
}

//Fonction servant à donner l'ordre lors de l'exécution au père de ne pas attendre son fils via la variable pass.
//Si le dernier charactère saisie par l'utilisateur est '&', ce charactère sera supprimer de la chaine, notre booléen pass va être retourner au main avec la valeur 1.
//Qui par la suite être utiliser dans la fonction d'exécution.
int background_verif(char* saisie)
{
	int pass = 0;
	if (saisie[strlen(saisie)-1] =='&')
	{
	 	saisie[strlen(saisie)-1] = '\0';
	 	pass = 1;
	}
	return pass;
}

// Fonction de fabication du prompt personnaliser selon la combinaison d'argument entrée 
int prompt(char** recup, int IR,char tab[])
{

	char *r, *u, h[255], heure[10], date[20], cwd[1024], argument[5];
	time_t t,d;
	int i, j, switch_value, compteur_i = 0, compteur_j = 0;	

	argument[0] = 'r';
	argument[1] = 't';
	argument[2] = 'd';
	argument[3] = 'h';
	argument[4] = 'u';		

	if (recup[1] == NULL)
	{
		r=getcwd(cwd, sizeof(cwd));				//initialisation du prompt si il n'y à pas eu de paramtre lors de l'exécution du programme
		u=getenv("USER");
		heure[0]='\0';
		date[0]='\0';
		h[0]='\0';	
	}
	else if (recup[1] != NULL)
	{
		r = getcwd(cwd, sizeof(cwd));
		u = getenv("USER");
		heure[0]='\0';
		date[0]='\0';
		h[0]='\0';
	
		for (i = 0; argument[i] != '\0'; i++)
		{
			compteur_i = compteur_i + 1;
		}
		for (j = 1; recup[1][j] != '\0'; j++)
		{
			compteur_j = compteur_j + 1;
		}

		if(recup[1][0] == '-' && recup[1][1] != '\0')
		{
			r = "";
			u = "";

			for (j = 0; j < compteur_j; j++)			//Comparaison entre chaque character entrée en paramètre, lors du lancement du programme
			{											//avec notre base de paramètre connue.
		
				for (i = 0; i < compteur_i; i++)
				{
					
					if ( argument[i] == recup[1][j+1] )
					{
						switch_value = i + 1;
						switch(switch_value)
						{
							case 1:
								r=getcwd(cwd, sizeof(cwd));	
					        	break;

					        case 2:
								t=time(NULL);
								strftime(heure, 20, "-%I:%M%p-", localtime(&t));
					        	break;

							case 3:
								d=time(NULL);
								strftime(date, 20, "-%x-", localtime(&d));
								break;

							case 4:
								gethostname(h,254);
								break;
							
							case 5:
								u=getenv("USER");	
								break;
						}
					}	
				}				
			}
		}
	}
	sprintf(tab,"\n%s:%s:\033[32;1m%s@%s\033[0m:SAM:\033[1;36m~%s\033[0m> ",heure,date,u,h,r);
}

//Fonction servant à récolter la saisie de l'utilisateur et permettant d'y accéder via les saisies flèches haut et bas
int takeInput(char* saisie, char tab[])
{
	char* buf;
	buf = readline(tab);
	if (strlen(buf) != 0)
	{
		add_history(buf);
		strcpy(saisie, buf);
		return 0;
	}
	else
	{
		return 1;
	}
}

int main(int argc, char* argv[])
{
	char saisie[MAX_INPUT_SZ], cwd[1024];
    char* backup[MAXCMD];
	char* cmd[MAXPARAM];

	char holdchdir[100][10];

	char tab[100];
	int boucle = 1, n = 0, i, u = 0, b = 0, z = 0, pass = 0, arret = 0, ret[2];

    for(i = 0; i < MAXCMD; i++)
    {
    	backup[i] = NULL;
    }

	//start_shell();
	printf("\nEn cas de soucis saisissez la commande 'help' (sans les quotes) pour accéder au manuel d'utilisation.");
	printf("\nEt si vous souhaitez quitter SÀM saisissez juste 'exit'.\n");

	while( boucle == 1)
	{
		prompt(argv,argc,tab);			//Affichage du prompt
		if (takeInput(saisie,tab))		//saisie de la commande utilisateur
			continue;
		printf("\n");
		
		pass = background_verif(saisie);	//Vérification si il y a eu demande d'exécution en arrière plan
		
		free(backup[u]);
		backup[u] = strdup(saisie);			// Duplication de la commande dans l'historique
		u = (u + 1) % MAXCMD;

		split_input(saisie, cmd);			//Découpage de la saisie de l'utilisateur
		
		switchcmd(cmd,backup,&b,u,holdchdir,&z,ret);		//Exécution des commandes personnalisées/ avec désactivation si besoin de la fonction d'execution des commandes normales
		//printf("%d", ret[0]);
		//z = ret[1];
		execution(cmd,b,pass);				//Exécution des commandes générales
		printf("z value in main: %d\n", z);
		printf("b value in main:%d \n",b);
		b = 0;						//débloquage fonction execution
		pass = 0;
	}
	return 0;
}