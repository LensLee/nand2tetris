#ifndef VMWRITER_H
#define VMWRITER_H
/**/
#include <stdio.h>
/**/
typedef unsigned char segment;
#define VMW_CONST 1
#define VMW_ARG 2
#define VMW_LOCAL 3
#define VMW_STATIC 4
#define VMW_THIS 5
#define VMW_THAT 6
#define VMW_POINTER 7
#define VMW_TEMP 8
/**/
typedef unsigned char command;
#define VMW_ADD 1
#define VMW_SUB 2
#define VMW_NEG 3
#define VMW_EQ 4
#define VMW_GT 5
#define VMW_LT 6
#define VMW_AND 7
#define VMW_OR 8
#define VMW_NOT 9
/**/
#define VMW_MULT 10
#define VMW_DIV 11

typedef struct{
	FILE *tar;//output target
}VMWriter;

/*Creates a new ouput .vm file
and prepares it for writing.*/
void setVMWriter(VMWriter*, FILE*);
/*Writes a VM push command.*/
void writePush(VMWriter*, segment, int);
/*Writes a VM pop command.*/
void writePop(VMWriter*, segment, int);

/*Writes a VM arithmetic-logical
command.*/
void writeArithmetic(VMWriter*, command);

/*Writes a VM label command.*/
void writeLabel(VMWriter*, char*);

/*Writes a VM goto command.*/
void writeGoto(VMWriter*, char*);

/*Writes a VM if-goto command.*/
void writeIf(VMWriter*, char*);

/*Writes a VM call command.*/
void writeCall(VMWriter*, char*, int);

/*Writes a VM function command.*/
void writeFunction(VMWriter*, char*, int);

/*Writes a VM return command.*/
void writeReturn(VMWriter*);

/*Closes the output file.*/
void closeVMWriter(VMWriter*);

#endif