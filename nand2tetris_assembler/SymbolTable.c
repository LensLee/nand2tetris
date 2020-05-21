#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "header.h"
/*SymbolTable: manages the symbol table*/
static struct Node **tables;
static struct Node *head;
static struct Node *last;
int memlocat=16;
/*Creates a new empty symbol table.*/
void addEntry(char*, int);
void Constructor(){
	tables=(struct Node**)malloc(sizeof(struct Node*));
	tables=&head;
	head=NULL;
	last=NULL;
	addEntry("R0",0);
	addEntry("R1",1);
	addEntry("R2",2);
	addEntry("R3",3);
	addEntry("R4",4);
	addEntry("R5",5);
	addEntry("R6",6);
	addEntry("R7",7);
	addEntry("R8",8);
	addEntry("R9",9);
	addEntry("R10",10);
	addEntry("R11",11);
	addEntry("R12",12);
	addEntry("R13",13);
	addEntry("R14",14);
	addEntry("R15",15);
	addEntry("SCREEN",16384);
	addEntry("KBD",24576);
	addEntry("SP",0);
	addEntry("LCL",1);
	addEntry("ARG",2);
	addEntry("THIS",3);
	addEntry("THAT",4);
}
/*Adds pair(symbol,address)to the table.*/
void addEntry(char *symbol, int address){
	struct Node *new=(struct Node*)malloc(sizeof(struct Node));
	strcpy(new->symbol, symbol);
	new->address=address;
	new->next=NULL;
	if(head==NULL){
		head=new;
		last=head;
		return;
	}
	last->next=new;
	last=last->next;
}
/*Does the symbol table contain the given symbol?*/
bool contains(char *symbol){
	struct Node *current=head;
	while(current){
		char *sb=current->symbol;
		if(!strcmp(sb,symbol))
			return true;
		current=current->next;
	}
	return false;
}
/*Returns the address associated with the symbol.*/
int getAddress(char *symbol){
	struct Node *current=head;
	while(current){
		char *sb=current->symbol;
		if(!strcmp(sb,symbol))
			return current->address;
		current=current->next;
	}
	return -1;
}
void display(){
	struct Node *current=head;
	while(current){
		printf("%s=%d\n", current->symbol, current->address);
		current=current->next;
	}
}