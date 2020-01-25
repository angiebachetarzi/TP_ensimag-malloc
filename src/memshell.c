/*****************************************************
 * Copyright Grégory Mounié 2013,2018                *
 *           Simon Nieuviarts 2008-2012              *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mem.h"

/*
  ===============================================================================
  Macros
  ===============================================================================
*/

/*
  #define DEBUG
*/

/*
 * Nombre max de blocs alloues en meme temps
 */
#define NB_MAX_ALLOC 5000
 
/*
 * Nombre de commandes differentes pour l'interpreteur
 * (sans inclure les commandes erronees (ERROR))
 */
#define NB_CMD 8
 
/*
 * Nombre de caracteres maximal pour une ligne de commande
 * (terminateur inclus A VERIFIER)
 */
#define MAX_CMD_SIZE 64

  
/* prompt */
#define PROMPT ">"

/*
  ===============================================================================
  Types
  ===============================================================================
*/

/*
 * Identification de la commande tapee
 * On rajoute ERROR pour les commandes erronees
 */
typedef enum {INIT=0, SHOW, USED, ALLOC, FREE, DESTROY, HELP, EXIT, ERROR} COMMAND;

/*
 * type des identificateurs de blocs
 */
typedef unsigned long ID;
 
  
/*
 * Identification des parametres tapes
 */
typedef struct {
	ID id;
	size_t size;
} ARG;
 
 
/*
 * Structure de donnees contenant les informations sur un bloc alloue
 */
typedef struct {
	/* identificateur du bloc alloue, vaut 0 si pas de bloc alloue*/
	ID id;
	/* adresse du bloc alloue, vaut NULL si pas de bloc alloue*/
	void *address;
	/* taille du bloc */
	size_t size; 
} BLOCINFO;

/*
  ===============================================================================
  Variables globales
  ===============================================================================
*/


/*
 * Compteur pour determiner les Ids des blocs alloues.
 * Doit etre (re)initialise a 1
 */
unsigned long id_count;

/*
 * Liste des commandes reconnues
 */
static char* commands[NB_CMD] = {"init", "show", "used","alloc", "free", "destroy", "help", "exit"};
	
	
/*
 * Tableau stockant les infos sur les blocs alloues
 * Les recherches dans cette table se font sur le champ id
 * des structures BLOCINFO
 */
BLOCINFO bloc_info_table[NB_MAX_ALLOC];

static void *zone_memoire;

/*
  ===============================================================================
  Fonctions
  ===============================================================================
*/


/*
 * Fonction d'affichage de la mémoire occupée
 */
void used()
{
	unsigned long i;
 	
	for(i=0; i<NB_MAX_ALLOC; i++) {
		if ((bloc_info_table[i]).id != 0) {
			printf("%ld 0x%lX 0x%lX\n",
			       bloc_info_table[i].id,
			       (unsigned long)(bloc_info_table[i].address - (void*)zone_memoire),
			       (unsigned long)bloc_info_table[i].size);
		}
	}
	
}


/*
 * Affichage de l'aide
 */
void help()
{
	printf("Commandes disponibles :\n");
	printf("1) init : initialisation ou réinitialisation de l'allocateur\n");
	printf("2) alloc <taille> : allocation d'un bloc mémoire\n");
	printf("\tLa taille peut être en décimal ou en héxadécimal (préfixe 0x)\n");
	printf("\tretour : identificateur de bloc et adresse de départ de la zone\n");
	printf("3) free <identificateur> : libération d'un bloc\n");
	printf("4) destroy : libération de l'allocateur\n");
	printf("4) show : affichage la taille initiale et de l'adresse de départ\n");	
	printf("5) used : affichage de la liste des blocs occupés\n");
	printf("\tsous la forme {identificateur, adresse de départ, taille}\n");		
	printf("6) help : affichage de ce manuel\n");
	printf("7) exit : quitter le shell\n");
	
	printf("\nRemarques :\n");
	printf("1) Au lancement, le shell appelle mem_init\n");
	printf("2) Le shell supporte jusqu'à %d allocations entre deux initialisations\n", NB_MAX_ALLOC);			
}

 
/*
 * Initialisation de l'interpreteur
 */
void init()
{

	int i;
 
	printf("**** Mini-shell de test pour l'allocateur mémoire ****\n");
	printf("\tTapez help pour la liste des commandes\n");
	
	id_count = 1;
	
	/* initialisation de la table des infos : */
	for (i=0; i < NB_MAX_ALLOC; i++) {
		(bloc_info_table[i]).id = 0;
	}

	printf("\n");
}

/*
 * Determine la commande tapee
 * token : chaine correspondant a la commande uniquement
 * cmd : emplacement ou disposer la commande eventuellement identifiee
 */
void get_command(char *token, COMMAND *cmd)
{
	
	COMMAND i;
	
	for(i=INIT; i<NB_CMD; i++) {
		if (!strcmp(token, commands[i])) break;
	}
	if (i < NB_CMD) /* si la commande a ete trouvee */
		*cmd = i;
	else *cmd=ERROR; /* sinon on signale l'erreur*/
}


/*
 * Determine les arguments de la ligne de commande
 * args : arguments tapes
 * pcmd : emplacement ou disposer les arguments identifies
 */
ARG get_args(char *args, COMMAND *pcmd)
{
	ARG our_args = {0,0};
	char *size_string, *id_string;
	long size, id;
	char *endptr= (char*)1;
		
	/* en fonction de la commande desiree */
	switch(*pcmd) {
		
	case ALLOC:
		if (args == NULL) {
			/* erreur si aucun argument */
			*pcmd = ERROR;
			break;
		}
		/* recuperation du parametre <taille> */
		size_string = strtok(args, "\n");
		
		size = strtol(size_string, &endptr, 0);
		/* NB : dernier parametre a 0 pour gerer decimal et hexa*/

		if ((*endptr != '\0') || (size == 0) || (size < 0)) {
			/* erreur si l'argument n'est pas entier 
			   ou s'il est nul, ou s'il est negatif */
			*pcmd = ERROR;
			break;
		}
		/* sinon l'argument est correct */
		/* on remplit la structure avec la valeur obtenue*/
		our_args.size = (size_t)size;				
		break;
				
	case FREE:
		id_string = strtok(args, "\n");
		if (id_string == NULL) {
			/* erreur si aucun argument */
			*pcmd = ERROR;
			break;
		}
		id = strtol(id_string, &endptr, 10);

		if ((*endptr != '\0') || (id == 0) || (id < 0)) {
			/* erreur si l'argument n'est pas entier
			   ou s'il est nul ou negatif */
			*pcmd = ERROR;
			break;
		}
		/* sinon l'argument est correct */
		/* on remplit la structure avec la valeur obtenue*/
		our_args.id = (ID)id;						
		break;
				
	default:;
	}
	return our_args;
}

 
/*
 * Analyse une ligne tapee
 * args : emplacement ou stocker la structure des arguments
 * retour : la commande tapee
 */
COMMAND read_command(ARG *args)
{
	//	char c;
	char cmd[MAX_CMD_SIZE]="";
	char *token;
	COMMAND our_cmd;
		
 
 	/* NB : il n'y a pas d'affichage du prompt */
	scanf("%[^\n]", cmd); /* lecture de la ligne de commande */
        getc(stdin); /* recuperation du \n */
	token = strtok(cmd," "); /* recuperation de la commande */
	get_command(token, &our_cmd); /* determination de la commande */
		
	/* si la commande a ete correctement identifiee, on obtient les arguments :*/
	if (our_cmd != ERROR) {
		token = strtok(NULL, "\n");
		*args = get_args(token, &our_cmd);
	}
		
	return our_cmd;	
}


/*
 * Obtient un identificateur a partir d'une adresse et d'une taille de bloc
 * et range les infos sur le bloc dans la table
 * addr : adresse du bloc
 * size : taille du bloc 
 * retour : un numero d'id ou 0 si plus d'id libre
 */
ID get_id(void *addr, size_t size)
{

	unsigned long index = 0;
 		
	while ((index < NB_MAX_ALLOC) && ((bloc_info_table[index]).id != 0)) {
		index++;
	}
		
	if (index == NB_MAX_ALLOC) { /* la limite d'allocation est atteinte */			
		return 0;
	}
	else {
			
		bloc_info_table[index].id = id_count;
		bloc_info_table[index].address = addr;
		bloc_info_table[index].size = size;
			
		return id_count++; /* NB: on postincremente id_count */
	}	
}


/*
 * Obtient la taille et l'adresse d'un bloc a partir d'un id
 * addr : emplacement ou stocker l'adresse du bloc
 * size : emplacement ou stocker la taille du bloc 
 * retour : 0 si ok, -1 si id incorrect
 */
int get_info_from_id(ID id, void** addr, size_t* size)
{
 
	unsigned long index = 0;
 
	/* si id invalide, echec */
	if (id < 1) return -1;
		
	while ((bloc_info_table[index].id != id) && (index < NB_MAX_ALLOC)) {
		index++;
	}
					
	/* si id non repertorie, echec */
	if (index == NB_MAX_ALLOC) return -1;
				 	
	*addr = bloc_info_table[index].address;
	*size = bloc_info_table[index].size;		

	return 0;		
}


 
/*
 * Libere l'entree associee a un id dans la table d'infos
 * On suppose que l'id existe dans la table
 * id : l'id a liberer
 */
void remove_id(ID id)
{
	unsigned long index = 0;
		
	while (bloc_info_table[index].id != id) {
		index++;
	}
			
	bloc_info_table[index].id = 0;
	bloc_info_table[index].address = NULL;
}

	
int main() {
	
	COMMAND cmd;
	ARG args = {0,0};
	void *res, *addr;
	ID id;
	size_t size;	

  
	init(); /* initialisation de l'interpreteur */
    	
	while(1) {
#ifdef DEBUG
		printf("memshell-main: debut de la boucle de l'interpreteur\n");
#endif
      		
		printf(PROMPT);
		
		cmd = read_command(&args);
		switch(cmd) {
			
		case INIT:
				
			printf("!!! Pas implanté dans ce sujet !!!\n");
			break;
			
		case SHOW:
		        printf("!!! Pas implanté dans ce sujet !!! Utilisez un débogueur !!!\n"); 
			break;

		case USED:
			used();
			break;
			
		case ALLOC:

			res = emalloc(args.size);
			/* si une erreur a lieu, on affiche 0 */
			if (res == NULL) {
				printf("Erreur : échec de l'allocation (fonction emalloc, retour=NULL)\n");
			} else {
				id = get_id(res, args.size);
				if (id == 0) {
					/* s'il ne reste pas d'id libre
					   on affiche 0 et on libere le bloc */
					printf("Erreur : nombre maximum d'allocations atteint/n");
					efree(res);
				} else { /* pas de probleme, affichage de la zone allouée */
					printf("%ld 0x%lX\n", id, (unsigned long)(res - (void*)zone_memoire));
				}
			}
			break;
				

		case DESTROY: 
			printf("!!! Pas implanté dans ce sujet !!!\n");
			break;

		case FREE:
						
			if (get_info_from_id(args.id, &addr, &size) == -1)
				/* erreur dans la valeur de l'id */
				printf("Erreur : identificateur de bloc incorrect\n"); 
			else {
							
							
				/* liberation du bloc concerne */
				efree(addr);
									
				/* liberation de l'id */
				remove_id(args.id);
								
				/* NB : dans le cas normal, on n'affiche rien */
			}
			break;
				
				
		case HELP:
			help();
			break;
				
		case EXIT:
			goto end;	
			
		case ERROR:
			
			printf("Commande incorrecte\n");
			break;
		}
			
	}
end: return 0;
}
