#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> 
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#define N 100 
#define COUNT 10

typedef struct {
	uint32_t prefix;
	uint32_t next_hop;
	uint32_t mask;
	int interface;
} __attribute__((packed)) Route_table_entry;


typedef struct cel{
	struct cel *left, *right;
	Route_table_entry *entry;
}*Node;


void addEntry(Node root, Route_table_entry entry);
Node initNode();


/*
	©Adrian Argint
	github.com/AdrianArgint		
								*/