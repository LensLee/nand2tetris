#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <libgen.h>
#include "mylib.h"

/*Parser.h*/
int Parser_init(FILE *);
bool hasMoreCommands();
void advance();
enum command_type commandType();
char *arg1();
int arg2();
/*CodeWriter.h*/
int CodeWriter_init(FILE *);
void writeArithmetic(char *);
void writePushPop(enum command_type, char *, int);
void Close();
void setFileName(char *);
void writeInit();
void writeLabel(char *);
void writeGoto(char *);
void writeIf(char *);
void writeFunction(char *, int);
void writeCall(char *, int);
void writeReturn();
/*API*/
static const char *inputFormat=".vm";
static const char *outputFormat=".asm";
void printCommand();

int main(int argc, char const *argv[]){
	char filePath[256]={'\0'};

	if(argv[1]!=NULL && argc==2){
		DIR *dp;
		struct dirent *ep;
		strcat(filePath, "./");
		strcat(filePath, argv[1]);
		strcat(filePath, "/");
		dp=opendir(filePath);
		char *b_name;
		b_name=basename((char *)argv[1]);

		if(dp!=NULL){
			FILE *input, *output;
			char outputName[256]={'\0'};
			strcat(outputName, b_name);
			strcat(outputName, outputFormat);
			output=fopen(outputName,"w");
			CodeWriter_init(output);
			if(INIT)
				writeInit();

			while(ep=readdir(dp)){
				int offset=strlen(ep->d_name)-strlen(inputFormat);

				if(offset>0){

					if(strcmp(ep->d_name+offset, inputFormat)==0){
						char file_name[256]={'\0'};
						strncpy(file_name, ep->d_name, offset);
						setFileName(file_name);
						char inputName[256]={'\0'};
						strcat(inputName,filePath);
						strcat(inputName,ep->d_name);

						if(input=fopen(inputName,"r")){
							Parser_init(input);

							while(hasMoreCommands()){
								advance();
								enum command_type command=commandType();
								char *_arg=arg1();
								int _arg2=arg2();

								if(command==C_ARITHMETIC)
									writeArithmetic(_arg);
								else if(command==C_PUSH||command==C_POP)
									writePushPop(command,_arg, _arg2);
								else if(command==C_LABEL)
									writeLabel(_arg);
								else if(command==C_GOTO)
									writeGoto(_arg);
								else if(command==C_IF)
									writeIf(_arg);

								else if(command==C_FUNCTION)
									writeFunction(_arg, _arg2);

								else if(command==C_RETURN)
									writeReturn();

								else if(command==C_CALL)
									writeCall(_arg, _arg2);
							}
							fclose(input);
						}
					}
				}
			}
			fseek(output,0,SEEK_END);

			if(ftell(output)==0){
				remove(outputName);
			}
			Close();
			closedir(dp);

			if(input==NULL){
				printf("No vm files found!\n");
				return -1;
			}
		}else if(strcmp(b_name+(strlen(b_name)-strlen(inputFormat)), inputFormat)==0){
			FILE *input, *output;
			char outputName[256]={'\0'};
			int offset=strlen(b_name)-strlen(inputFormat);
			char file_name[256]={'\0'};
			strncpy(file_name, b_name, offset);
			setFileName(file_name);
			strcat(outputName, file_name);
			strcat(outputName, outputFormat);
			output=fopen(outputName,"w");
			if(INIT)
				writeInit();

			if(input=fopen(argv[1],"r")){
				Parser_init(input);
				CodeWriter_init(output);
				while(hasMoreCommands()){
					advance();
					enum command_type command=commandType();
					char *_arg=arg1();
					int _arg2=arg2();

					if(command==C_ARITHMETIC)
						writeArithmetic(_arg);
					else if(command==C_PUSH||command==C_POP)
						writePushPop(command,_arg, _arg2);
					else if(command==C_LABEL)
						writeLabel(_arg);
					else if(command==C_GOTO)
						writeGoto(_arg);
					else if(command==C_IF)
						writeIf(_arg);

					else if(command==C_FUNCTION)
						writeFunction(_arg, _arg2);

					else if(command==C_RETURN)
						writeReturn();

					else if(command==C_CALL)
						writeCall(_arg, _arg2);

				}
			}else{
				perror ("Couldn't open the file");
			}
			Close();
			fclose(input);
		}
	}
	return 0;
}