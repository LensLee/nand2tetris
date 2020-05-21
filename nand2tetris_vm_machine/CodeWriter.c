#include <stdio.h>
#include <string.h>
#include "mylib.h"
static FILE *output;
static char _fileName[256]={'\0'};
static char _functionName[256]={'\0'};
enum command_type commandType();
/*Opens the output file/stream and gets ready to write into it.*/
static void Constructor(FILE *stream){
		output=stream;
}
/*API*/
int CodeWriter_init(FILE *outputFile){
	if(outputFile){		
		Constructor(outputFile);
		return 1;
	}else{
		return 0;
	}
}
/*Writes to the output file the assembly code that
implements the given arithmetic command.*/
void writeArithmetic(char *command){
	static int bool_label_count=0;
	if(strcmp(command,"add")==0
		||strcmp(command,"sub")==0
		||strcmp(command,"and")==0
		||strcmp(command,"or")==0
		||strcmp(command,"eq")==0
		||strcmp(command,"gt")==0
		||strcmp(command,"lt")==0){
		fprintf(output,"@SP\n");
		fprintf(output,"M=M-1\n");
		fprintf(output,"A=M\n");
		fprintf(output,"D=M\n");
		fprintf(output,"@SP\n");
		fprintf(output,"M=M-1\n");
		fprintf(output,"A=M\n");

		if(strcmp(command,"add")==0)
			fprintf(output,"D=M+D\n");

		else if(strcmp(command,"sub")==0
				||strcmp(command,"eq")==0
				||strcmp(command,"gt")==0
				||strcmp(command,"lt")==0){
				fprintf(output,"D=M-D\n");

			if(strcmp(command,"eq")==0
				||strcmp(command,"gt")==0
				||strcmp(command,"lt")==0){
				fprintf(output,"@BOOLEAN%d\n", bool_label_count);

				if(strcmp(command,"eq")==0)
					fprintf(output,"D;JEQ\n");

				else if(strcmp(command,"gt")==0)
					fprintf(output,"D;JGT\n");

				else if(strcmp(command,"lt")==0)
					fprintf(output,"D;JLT\n");

				fprintf(output,"D=0\n");
				fprintf(output,"@BOOLEAN%d\n", bool_label_count+1);
				fprintf(output,"0;JMP\n");
				fprintf(output,"(BOOLEAN%d)\n", bool_label_count);
				fprintf(output,"D=-1\n");
				fprintf(output,"(BOOLEAN%d)\n", bool_label_count+1);
				fprintf(output,"@SP\n");
				fprintf(output,"A=M\n");
				bool_label_count+=2;
			}
		}
		else if(strcmp(command,"and")==0)
			fprintf(output,"D=M&D\n");

		else if(strcmp(command,"or")==0)
			fprintf(output,"D=M|D\n");

		fprintf(output,"M=D\n");
		fprintf(output,"@SP\n");
		fprintf(output,"M=M+1\n");
	}else if(strcmp(command,"neg")==0||strcmp(command,"not")==0){
		fprintf(output,"@SP\n");
		fprintf(output,"M=M-1\n");
		fprintf(output,"A=M\n");

		if(strcmp(command,"neg")==0)
			fprintf(output,"D=-M\n");

		else if(strcmp(command,"not")==0)
			fprintf(output,"D=!M\n");

		fprintf(output,"M=D\n");
		fprintf(output,"@SP\n");
		fprintf(output,"M=M+1\n");
	}
}
/*Writes to the output file the assembly code that implements the given
command, where command is either C_PUSH or C_POP.*/
void writePushPop(enum command_type command, char *segment, int index){
	if(command==C_PUSH){
		if(strcmp(segment,"constant")==0){
			fprintf(output,"@%d\n",index);
			fprintf(output,"D=A\n");			
		}else if(strcmp(segment,"local")==0){
			fprintf(output,"@%d\n",index);
			fprintf(output,"D=A\n");
			fprintf(output,"@LCL\n");
			fprintf(output,"A=D+M\n");
			fprintf(output,"D=M\n");
		}else if(strcmp(segment,"static")==0){

			if(strlen(_functionName)!=0)
				fprintf(output,"@%s.%d\n",_functionName, index);

			else
				fprintf(output,"@%s.%d\n",_fileName, index);

			fprintf(output,"D=M\n");
		}else if(strcmp(segment,"temp")==0){
			fprintf(output,"@5\n");
			fprintf(output,"D=A\n");
			fprintf(output,"@%d\n",index);
			fprintf(output,"A=D+A\n");
			fprintf(output,"D=M\n");
		}else if(strcmp(segment,"pointer")==0){
			if(index==0)
				fprintf(output,"@THIS\n");

			else if(index==1)
				fprintf(output,"@THAT\n");

			fprintf(output,"D=M\n");
		}else if(strcmp(segment,"this")==0||strcmp(segment,"that")==0){
			fprintf(output,"@%d\n",index);
			fprintf(output,"D=A\n");

			if(strcmp(segment,"this")==0)
				fprintf(output,"@THIS\n");

			else if(strcmp(segment,"that")==0)
				fprintf(output,"@THAT\n");

			fprintf(output,"A=D+M\n");
			fprintf(output,"D=M\n");
		}else if(strcmp(segment,"argument")==0){
			fprintf(output,"@%d\n",index);
			fprintf(output,"D=A\n");
			fprintf(output,"@ARG\n");
			fprintf(output,"A=D+M\n");
			fprintf(output,"D=M\n");
		}
		fprintf(output,"@SP\n");
		fprintf(output,"A=M\n");
		fprintf(output,"M=D\n");
		fprintf(output,"@SP\n");
		fprintf(output,"M=M+1\n");
	}else if(command==C_POP){
		if(strcmp(segment,"local")==0){
			fprintf(output,"@%d\n",index);
			fprintf(output,"D=A\n");
			fprintf(output,"@LCL\n");
			fprintf(output,"D=D+M\n");
			fprintf(output,"@R13\n");
			fprintf(output,"M=D\n");
			fprintf(output,"@SP\n");
			fprintf(output,"M=M-1\n");
			fprintf(output,"A=M\n");
			fprintf(output,"D=M\n");
			fprintf(output,"@R13\n");
			fprintf(output,"A=M\n");				
			fprintf(output,"M=D\n");
		}else if(strcmp(segment,"static")==0){
			fprintf(output,"@SP\n");
			fprintf(output,"M=M-1\n");
			fprintf(output,"A=M\n");
			fprintf(output,"D=M\n");

			if(strlen(_functionName)!=0)
				fprintf(output,"@%s.%d\n",_functionName, index);

			else
				fprintf(output,"@%s.%d\n",_fileName, index);
			
			fprintf(output,"M=D\n");
		}else if(strcmp(segment,"temp")==0){
			fprintf(output,"@5\n");
			fprintf(output,"D=A\n");
			fprintf(output,"@%d\n", index);
			fprintf(output,"D=D+A\n");
			fprintf(output,"@R13\n");
			fprintf(output,"M=D\n");
			fprintf(output,"@SP\n");
			fprintf(output,"M=M-1\n");
			fprintf(output,"A=M\n");
			fprintf(output,"D=M\n");
			fprintf(output,"@R13\n");
			fprintf(output,"A=M\n");				
			fprintf(output,"M=D\n");
		}else if(strcmp(segment,"pointer")==0){
			fprintf(output,"@SP\n");
			fprintf(output,"M=M-1\n");
			fprintf(output,"A=M\n");
			fprintf(output,"D=M\n");

			if(index==0)
				fprintf(output,"@THIS\n");

			else if(index==1)
				fprintf(output,"@THAT\n");

			fprintf(output,"M=D\n");		
		}else if(strcmp(segment,"this")==0||strcmp(segment,"that")==0){
			fprintf(output,"@%d\n",index);
			fprintf(output,"D=A\n");

			if(strcmp(segment,"this")==0)
				fprintf(output,"@THIS\n");

			else if(strcmp(segment,"that")==0)
				fprintf(output,"@THAT\n");

			fprintf(output,"D=D+M\n");
			fprintf(output,"@R13\n");
			fprintf(output,"M=D\n");
			fprintf(output,"@SP\n");
			fprintf(output,"M=M-1\n");
			fprintf(output,"A=M\n");
			fprintf(output,"D=M\n");
			fprintf(output,"@R13\n");
			fprintf(output,"A=M\n");				
			fprintf(output,"M=D\n");
		}else if(strcmp(segment,"argument")==0){
			fprintf(output,"@%d\n",index);
			fprintf(output,"D=A\n");
			fprintf(output,"@ARG\n");
			fprintf(output,"D=D+M\n");
			fprintf(output,"@R13\n");
			fprintf(output,"M=D\n");
			fprintf(output,"@SP\n");
			fprintf(output,"M=M-1\n");
			fprintf(output,"A=M\n");
			fprintf(output,"D=M\n");
			fprintf(output,"@R13\n");
			fprintf(output,"A=M\n");
			fprintf(output,"M=D\n");
		}
	}
}
/*Informs the codeWriter that the translation of a new VM file has
started (called by the main program of the VM translator).*/
void setFileName(char *fileName){
	_fileName[0]='\0';
	_functionName[0]='\0';
	strcpy(_fileName, fileName);
}
/*Writes the assembly instructions that effect the bootstrap code that initializes the VM.
This code must be placed at the beginning of the generated *.asm file.*/
void writeInit(){
	fprintf(output,"@261\n");
	fprintf(output,"D=A\n");
	fprintf(output,"@SP\n");
	fprintf(output,"M=D\n");

	fprintf(output,"@Sys.init\n");
	fprintf(output,"0;JMP\n");
}
/*Writes assembly code that effects the label command.*/
void writeLabel(char *label){
	if(strlen(_functionName)!=0)
		fprintf(output,"(%s$%s)\n", _functionName, label);
	else
	fprintf(output,"(%s)\n", label);
}
/*Writes assembly code that effects the goto command.*/
void writeGoto(char *label){
	if(strlen(_functionName)!=0)
		fprintf(output,"@%s$%s\n", _functionName, label);
	else
		fprintf(output,"@%s\n", label);
	
	fprintf(output,"0;JMP\n");
}
/*Writes assembly code that effects the if-goto command.*/
void writeIf(char *label){
	static int if_label_count=0;
	fprintf(output,"@SP\n");
	fprintf(output,"M=M-1\n");
	fprintf(output,"A=M\n");
	fprintf(output,"D=M\n");
	fprintf(output,"@IF%d\n", if_label_count);
	fprintf(output,"D;JEQ\n");

	if(strlen(_functionName)!=0)
		fprintf(output,"@%s$%s\n", _functionName, label);
	else
		fprintf(output,"@%s\n", label);

	fprintf(output,"0;JMP\n");
	fprintf(output,"(IF%d)\n", if_label_count++);
}
/*Writes assembly code that effects the function command.*/
void writeFunction(char *functionName, int numVars){
	if(strlen(_functionName)==0){
		_functionName[0]='\0';
		strcpy(_functionName, functionName);
	}

	fprintf(output,"(%s)\n", functionName);

	for(int i=0; i<numVars; i++){
		fprintf(output,"@SP\n");
		fprintf(output,"A=M\n");
		fprintf(output,"M=0\n");
		fprintf(output,"@SP\n");
		fprintf(output,"M=M+1\n");
	}
}
/*Writes assembly code that effects the call command.*/
void writeCall(char *functionName, int numArgs){
	static int return_label_count=0;
	fprintf(output, "@%s$ret.%d\n", functionName, return_label_count);
	fprintf(output,"D=A\n");
	fprintf(output,"@SP\n");
	fprintf(output,"A=M\n");
	fprintf(output,"M=D\n");
	fprintf(output,"@SP\n");
	fprintf(output,"M=M+1\n");

	fprintf(output,"@LCL\n");
	fprintf(output,"D=M\n");
	fprintf(output,"@SP\n");
	fprintf(output,"A=M\n");
	fprintf(output,"M=D\n");
	fprintf(output,"@SP\n");
	fprintf(output,"M=M+1\n");

	fprintf(output,"@ARG\n");
	fprintf(output,"D=M\n");
	fprintf(output,"@SP\n");
	fprintf(output,"A=M\n");
	fprintf(output,"M=D\n");
	fprintf(output,"@SP\n");
	fprintf(output,"M=M+1\n");

	fprintf(output,"@THIS\n");
	fprintf(output,"D=M\n");
	fprintf(output,"@SP\n");
	fprintf(output,"A=M\n");
	fprintf(output,"M=D\n");
	fprintf(output,"@SP\n");
	fprintf(output,"M=M+1\n");

	fprintf(output,"@THAT\n");
	fprintf(output,"D=M\n");
	fprintf(output,"@SP\n");
	fprintf(output,"A=M\n");
	fprintf(output,"M=D\n");
	fprintf(output,"@SP\n");
	fprintf(output,"M=M+1\n");

	fprintf(output,"@SP\n");
	fprintf(output,"D=M\n");
	fprintf(output,"@5\n");
	fprintf(output,"D=D-A\n");
	fprintf(output,"@%d\n", numArgs);
	fprintf(output,"D=D-A\n");
	fprintf(output,"@ARG\n");
	fprintf(output,"M=D\n");

	fprintf(output,"@SP\n");
	fprintf(output,"D=M\n");
	fprintf(output,"@LCL\n");
	fprintf(output,"M=D\n");

	fprintf(output,"@%s\n", functionName);
	fprintf(output,"0;JMP\n");

	fprintf(output, "(%s$ret.%d)\n", functionName, return_label_count++);
}
/*Writes assembly code that effects the return command.*/
void writeReturn(){

	fprintf(output,"@LCL\n");
	fprintf(output,"D=M\n");
	fprintf(output,"@R13\n");
	fprintf(output,"M=D\n");//save endFrame

	fprintf(output,"@5\n");
	fprintf(output,"A=D-A\n");
	fprintf(output,"D=M\n");
	fprintf(output,"@R14\n");
	fprintf(output,"M=D\n");//save return address

	fprintf(output,"@SP\n");
	fprintf(output,"M=M-1\n");
	fprintf(output,"A=M\n");
	fprintf(output,"D=M\n");
	fprintf(output,"@ARG\n");
	fprintf(output,"A=M\n");
	fprintf(output,"M=D\n");//save return value

	fprintf(output,"@ARG\n");
	fprintf(output,"D=M+1\n");
	fprintf(output,"@SP\n");
	fprintf(output,"M=D\n");//reposition SP

	fprintf(output,"@1\n");
	fprintf(output,"D=A\n");
	fprintf(output,"@R13\n");
	fprintf(output,"A=M-D\n");
	fprintf(output,"D=M\n");
	fprintf(output,"@THAT\n");
	fprintf(output,"M=D\n");

	fprintf(output,"@2\n");
	fprintf(output,"D=A\n");
	fprintf(output,"@R13\n");
	fprintf(output,"A=M-D\n");
	fprintf(output,"D=M\n");
	fprintf(output,"@THIS\n");
	fprintf(output,"M=D\n");

	fprintf(output,"@3\n");
	fprintf(output,"D=A\n");
	fprintf(output,"@R13\n");
	fprintf(output,"A=M-D\n");
	fprintf(output,"D=M\n");
	fprintf(output,"@ARG\n");
	fprintf(output,"M=D\n");

	fprintf(output,"@4\n");
	fprintf(output,"D=A\n");
	fprintf(output,"@R13\n");
	fprintf(output,"A=M-D\n");
	fprintf(output,"D=M\n");
	fprintf(output,"@LCL\n");
	fprintf(output,"M=D\n");

	fprintf(output,"@R14\n");
	fprintf(output,"A=M\n");
	fprintf(output,"0;JMP\n");//return
}
/*Closes the output file.*/
void Close(){
	if(fclose(output));
}