#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "header.h"
#include <ctype.h>
/*Parser: unpacks each instruction into its underlying fields*/
static FILE* input;
static char command[256]={'\0'};
static char _symbol[256]={'\0'};
static int eof;
int cur_pos=-1;
/*Opens the input/stream and gets ready to parse it.*/
void Initializer(FILE* inputFile){
	cur_pos=-1;
	input=inputFile;
	fseek(input,0,SEEK_END);//Get the End value of file
	eof=ftell(input);
	fseek(input,0,SEEK_SET);//Reset to zero
}
/*Are there more lines in the input?*/
bool hasMoreCommands(){
	return ftell(input)!=eof;
}
/*Reads the next command from the input,
and makes it the current command.
Takes care of whitespace,if necessary.
Should be called only if hasMoreCommands() is true.
Initially there is no current command.*/
enum command_type commandType();
char* symbol();
void advance(){
	do{
		char tmp[256]={'\0'};
		if(fgets(command,256,input)){
			sscanf(command,"%[^/\r]",tmp);//Clear Empty lines, Comments. CR(\r, 13), LF(\n, 10)
			int i,j;
			for(i=0, j=0; i<strlen(tmp); i++)//Hdndle WhiteSpace
				if(tmp[i]!=' ')
					command[j++]=tmp[i];
			command[j]='\0';
			if(strlen(command)){
				if(commandType()!=L_COMMAND)
					cur_pos++;
			}
		}
	}while(!strlen(command));
}
/*Returns the type of the current command:
A_COMMAND for @xxx Where xxx is either
a symbol or a decimal number
C_COMMAND for dest=comp;jump
L_COMMAND for (xxx) where xxx is a symbol.*/
enum command_type commandType(){
	enum command_type type;
	if(strlen(command)){
		if(command[0]=='@'){
			type=A_COMMAND;				
			return type;
		}else if(command[0]=='('&&command[strlen(command)-1]==')'){
			type=L_COMMAND;
		}else{
			type=C_COMMAND;
		}
		return type;
	}
	type=NO_COMMAND;
	return type;
}
/*Returns the symbol or decimal xxx of the current
command @xxx or (xxx)
Should be called only when commandType()
is A_COMMAND or L_COMMAND.*/
char* symbol(){
	_symbol[0]='\0';
	if(commandType()==A_COMMAND){
		sscanf(command,"@%s", _symbol);
		return _symbol;
	}else if(commandType()==L_COMMAND){
		sscanf(command,"(%[^)]", _symbol);
		return _symbol;
	}
	return NULL;
}
/*Returns the dest mnemonic in the current
C-command(8 possibilities).
Should be called only when commandType()
is C_COMMAND*/
char* parser_dest(){
	_symbol[0]='\0';
	if(commandType()==C_COMMAND){
		if(strchr(command,'=')){
			sscanf(command,"%[^=]=%*s",_symbol);
			return _symbol;
		}
	}
	return NULL;
}
/*Returns the comp mnemonic in the current
C-command(28 possibilities).
Should be called only when commandType()
is C_COMMAND*/
char* parser_comp(){
	_symbol[0]='\0';
	if(commandType()==C_COMMAND){
		if(strchr(command,'=')){
			sscanf(command,"%*[^=]=%s",_symbol);
			return _symbol;
		}else if(strchr(command,';')){
			sscanf(command,"%[^;];%*s",_symbol);
			return _symbol;
		}
	}
	return NULL;
}
/*Returns the jump mnemonic in the current
C-command(8 possibilities).
Should be called only when commandType()
is C_COMMAND*/
char* parser_jump(){
	_symbol[0]='\0';
	if(commandType()==C_COMMAND){
		if(strchr(command,';')){
			sscanf(command,"%*[^;];%s",_symbol);
			return _symbol;
		}
	}
	return NULL;
}