#ifndef HEADER
#define HEADER
enum command_type{A_COMMAND, C_COMMAND, L_COMMAND, NO_COMMAND=-1};
struct Node{
	char symbol[48];
	int address;
	struct Node *next;	
};
#endif