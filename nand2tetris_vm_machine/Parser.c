#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "mylib.h"
static FILE *input;
static int eof;
static char command[256]={'\0'};
static char arg[256]={'\0'};
/*Opens the input file/stream and gets ready to parse it.*/
static void Constructor(FILE *stream){
	input=stream;
	fseek(input, 0, SEEK_END);
	eof=ftell(input);
	fseek(input, 0, SEEK_SET);
}
/*API*/
int Parser_init(FILE *inputFile){
	if(inputFile){		
		Constructor(inputFile);
		return 1;
	}else{
		return 0;
	}
}
void printCommand(){
	printf("%s\n", command);
}
/*Are there more commands in the input?*/
bool hasMoreCommands(){
	return ftell(input)!=eof; 
}
/*Reads the next command from the input and makes it the current command.
Should be called only if hasMoreCommands() is true.
Initially there is no current command.*/
void advance(){
	command[0]='\0';
	do{
		char tmp[256]={'\0'};
		if(fgets(tmp,256,input))
			sscanf(tmp,"%[^/\r]",command);

	}while(strlen(command)==0);
}
/*Returns a constant representing the type of the current command.
C_ARITHMETIC is returned for all the arithmetic/logical commands.*/
enum command_type commandType(){
	if(strlen(command)){
		int len=strlen(command);
		char tmp[len+1];
		strcpy(tmp,command);
		char *token=strtok(tmp," ");
		if(strcmp(token, "push")==0){
			return C_PUSH;
		}else if(strcmp(token, "pop")==0){
			return C_POP;
		}else if(strcmp(token, "add")==0
				||strcmp(token, "sub")==0
				||strcmp(token, "neg")==0
				||strcmp(token, "eq")==0
				||strcmp(token, "gt")==0
				||strcmp(token, "lt")==0
				||strcmp(token, "and")==0
				||strcmp(token, "or")==0
				||strcmp(token, "not")==0){
			return C_ARITHMETIC;
		}else if(strcmp(token, "label")==0){
			return C_LABEL;
		}else if(strcmp(token, "goto")==0){
			return C_GOTO;			
		}else if(strcmp(token, "if-goto")==0){
			return C_IF;
		}else if(strcmp(token, "call")==0){
			return C_CALL;			
		}else if(strcmp(token, "function")==0){
			return C_FUNCTION;	
		}else if(strcmp(token, "return")==0){
			return C_RETURN;
		}
	}
}
/*Returns the first argument of the current command.
In the case of C_ARITHMETIC, the command itself(add,sub,etc.)is returned.
Should not be called if the current command is C_RETURN.*/
char *arg1(){
	if(commandType()==C_ARITHMETIC){
		int len=strlen(command);
		char tmp[len+1];
		strcpy(tmp,command);
		char *token=strtok(tmp," ");
		arg[0]='\0';
		strcpy(arg,token);
		return arg;
	}else if(commandType()==C_PUSH
			||commandType()==C_POP
			||commandType()==C_LABEL
			||commandType()==C_GOTO
			||commandType()==C_IF
			||commandType()==C_FUNCTION
			||commandType()==C_CALL){
		int len=strlen(command);
		char tmp[len+1];
		strcpy(tmp,command);
		char *token=strtok(tmp," ");
		token=strtok(NULL," ");
		arg[0]='\0';
		strcpy(arg,token);
		return arg;
	}
	return NULL;
}
/*Returns the second argument of the current command.
Should be called only if the current command is C_PUSH,
C_POP, C_FUNCTION, or C_CALL.*/
int arg2(){
	if(commandType()==C_PUSH
		||commandType()==C_POP
		||commandType()==C_FUNCTION
		||commandType()==C_CALL){
		int len=strlen(command);
		char tmp[len+1];
		strcpy(tmp,command);
		char *token=strtok(tmp," ");
		token=strtok(NULL," ");
		token=strtok(NULL," ");
		int n;
		if(sscanf(token, "%d", &n))
			return n;
	}else{
		return -1;
	}
}