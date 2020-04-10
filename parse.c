#include "parse.h"
#include "tree.h"

/* Am "reinventat" functia de comparare */
uint32_t ip(char *str){
	uint8_t byte1, byte2, byte3, byte4;
	sscanf(str, "%hhd.%hhd.%hhd.%hhd", &byte1, &byte2, &byte3, &byte4);
	return (byte1 << 24) + (byte2 << 16) + (byte3 << 8) + byte4;
}



/* Functia de citire a tabele de rutare */
Node read_route_table(){
	Node root = initNode();
	if(!root) return NULL;


	FILE *fin = fopen("rtable.txt", "r");
	if(!fin){
		free(root);
		return NULL;
	}

	
	char *line = (char *) malloc(50 * sizeof(char));
	/* Citire line cu line in introducere in arbore a fiecarui nod */
	while(fgets(line, 50, fin) != NULL)
	{
		Route_table_entry rtable;
		char *token;
		
		/*first column*/ 
		token = strtok(line, " ");
		rtable.prefix = ip(token);
		
		/*second column*/ 	
		token = strtok(NULL, " ");
		rtable.next_hop = ip(token);
		/*third column*/ 
		token = strtok(NULL, " ");
		rtable.mask = ip(token);
		
		/*fourth column*/ 
		token = strtok(NULL, " ");
		sscanf(token, "%d", &rtable.interface);
		addEntry(root, rtable);
	}


	return root;


}



/*
	©Adrian Argint
	github.com/AdrianArgint		
								*/