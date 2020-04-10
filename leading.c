#include "leading.h"

/* Metoda de a afla cea mai specifica adresa de next_hop */
Route_table_entry* get_best_route(Node root, uint32_t ip){
	// printf("ip = %u\n", ip);
	uint32_t mask = 1 << 31;
	Node ant = NULL;
	int count = 1;
	while(root && mask){
		// printf("count = %d\n", count);
		count++;
		ant = root;
		if(mask & ip)
			root= root->right;
		else
			root = root->left;
		mask = mask >> 1;
	}


	return ant->entry;
}



/*
	©Adrian Argint
	github.com/AdrianArgint		
								*/