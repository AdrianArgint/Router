#include "tree.h"

/* initializare nod */
Node initNode(){
	Node aux = (Node) malloc(sizeof(struct cel));
	if(!aux) return	NULL;
	aux->left = NULL;
	aux->right = NULL;
	aux->entry = NULL;
	return aux;
}


Node addEntryAux(Node root, uint32_t prefix, uint32_t mask, int level){
	//stop cand se ajunge mask0
	if(!mask)
		return root;

	//calea pe care o sa mearga
	//0 - stanga 	1 - dreapta
	
	//masca trebuie shiftata cu 1 la stanga si level-ul scazut cu 1
	if ((1 << level) & prefix) {
		if(root->right == NULL)
			root->right = initNode();
		return	addEntryAux(root->right, prefix, mask << 1, level - 1);
	}
	if(root->left == NULL)
		root->left = initNode();

	return addEntryAux(root->left, prefix, mask << 1, level - 1);
}

/* copiaza informatia dintr-o variabila de tip tabela de rutare intr-un nod pe care-l intoarce*/
void copyRouteInfo(Node node, Route_table_entry entry){
	node->entry = (Route_table_entry *) malloc(sizeof(Route_table_entry));
	node->entry->prefix = entry.prefix;
	node->entry->next_hop = entry.next_hop;
	node->entry->mask = entry.mask;
	node->entry->interface = entry.interface;
}

/* Adauga un nod nou cu informatii de tipul tabela de rutare in arbore */
void addEntry(Node root, Route_table_entry entry){
	Node node = addEntryAux(root, entry.prefix, entry.mask, 31);
	copyRouteInfo(node, entry);
}



/*
	©Adrian Argint
	github.com/AdrianArgint		
								*/