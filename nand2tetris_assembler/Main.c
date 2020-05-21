/*Main: initializes I/O files and drives the process.*/
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "header.h"
#include <ctype.h>

void Initializer(FILE*);
bool hasMoreCommands();
void advance();
enum command_type commandType();
char* symbol();
char* parser_dest();
char* parser_comp();
char* parser_jump();
char* code_dest(char*);
char* code_comp(char*);
char* code_jump(char*);
void Constructor();
void addEntry(char*,int);
bool contains(char*);
int getAddress(char*);
extern int memlocat;
void display();
extern int cur_pos;

static char instruction[17]={'\0'};


int main(){
	FILE* fp=fopen("Pong.asm","r");
	FILE* fp2=fopen("Pong.hack","w");
	Initializer(fp);
	Constructor();
	while(hasMoreCommands()){
		advance();
		if(commandType()==L_COMMAND){
			char *label=symbol();
			addEntry(label,cur_pos+1);
		}
	}

	Initializer(fp);

	while(hasMoreCommands()){
		advance();
		if(commandType()==A_COMMAND){
			char *str=symbol();
			if(str!=NULL){
				int n;
				if(sscanf(str,"%d",&n));
				else{
					if(!contains(str)){
						n=memlocat;
						addEntry(str,memlocat++);
					}else{
						n=getAddress(str);
					}
				}
				instruction[0]='0';
				instruction[1]=((0x4000&n)>>14)+'0';
				instruction[2]=((0x2000&n)>>13)+'0';
				instruction[3]=((0x1000&n)>>12)+'0';
				instruction[4]=((0x0800&n)>>11)+'0';
				instruction[5]=((0x0400&n)>>10)+'0';
				instruction[6]=((0x0200&n)>>9)+'0';
				instruction[7]=((0x0100&n)>>8)+'0';
				instruction[8]=((0x0080&n)>>7)+'0';
				instruction[9]=((0x0040&n)>>6)+'0';
				instruction[10]=((0x0020&n)>>5)+'0';
				instruction[11]=((0x0010&n)>>4)+'0';
				instruction[12]=((0x0008&n)>>3)+'0';
				instruction[13]=((0x0004&n)>>2)+'0';
				instruction[14]=((0x0002&n)>>1)+'0';
				instruction[15]=(0x0001&n)+'0';
				instruction[16]='\0';
				fprintf(fp2,"%s\n", instruction);
			}
		}else if(commandType()==C_COMMAND){
			instruction[0]='1';
			instruction[1]='1';
			instruction[2]='1';
			instruction[3]='\0';
			char* tmp=parser_dest();
			char d[4],c[8],j[4];
			strcpy(d,code_dest(tmp));
			d[3]='\0';	
			tmp=parser_comp();
			strcpy(c,code_comp(tmp));				
			c[7]='\0';	
			tmp=parser_jump();
			strcpy(j,code_jump(tmp));
			j[3]='\0';	
			strcat(instruction,c);							
			strcat(instruction,d);							
			strcat(instruction,j);
			// printf("%s\n", instruction);
			fprintf(fp2,"%s\n", instruction);
		}
	}
	// display();
	return 0;
}