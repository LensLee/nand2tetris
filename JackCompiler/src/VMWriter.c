#include "..\include\VMWriter.h"

void setVMWriter(VMWriter *vmw, FILE *tar){
	vmw->tar = tar;
}
void writePush(VMWriter *vmw, segment seg, int idx){
	switch(seg){
		case VMW_CONST:
			fprintf(vmw->tar, "push constant %d\n", idx);
			break;
		case VMW_ARG:
			fprintf(vmw->tar, "push argument %d\n", idx);
			break;
		case VMW_LOCAL:
			fprintf(vmw->tar, "push local %d\n", idx);
			break;
		case VMW_STATIC:
			fprintf(vmw->tar, "push static %d\n", idx);
			break;
		case VMW_THIS:
			fprintf(vmw->tar, "push this %d\n", idx);
			break;
		case VMW_THAT:
			fprintf(vmw->tar, "push that %d\n", idx);
			break;
		case VMW_POINTER:
			fprintf(vmw->tar, "push pointer %d\n", idx);
			break;
		case VMW_TEMP:
			fprintf(vmw->tar, "push temp %d\n", idx);
			break;
	}
}
void writePop(VMWriter *vmw, segment seg, int idx){
	switch(seg){
		case VMW_ARG:
			fprintf(vmw->tar, "pop argument %d\n", idx);
			break;
		case VMW_LOCAL:
			fprintf(vmw->tar, "pop local %d\n", idx);
			break;
		case VMW_STATIC:
			fprintf(vmw->tar, "pop static %d\n", idx);
			break;
		case VMW_THIS:
			fprintf(vmw->tar, "pop this %d\n", idx);
			break;
		case VMW_THAT:
			fprintf(vmw->tar, "pop that %d\n", idx);
			break;
		case VMW_POINTER:
			fprintf(vmw->tar, "pop pointer %d\n", idx);
			break;
		case VMW_TEMP:
			fprintf(vmw->tar, "pop temp %d\n", idx);
			break;
	}
}
void writeArithmetic(VMWriter *vmw, command cmd){
	switch(cmd){
		case VMW_ADD:
			fprintf(vmw->tar, "add\n");
			break;
		case VMW_SUB:
			fprintf(vmw->tar, "sub\n");
			break;
		case VMW_NEG:
			fprintf(vmw->tar, "neg\n");
			break;
		case VMW_EQ:
			fprintf(vmw->tar, "eq\n");
			break;
		case VMW_GT:
			fprintf(vmw->tar, "gt\n");
			break;
		case VMW_LT:
			fprintf(vmw->tar, "lt\n");
			break;
		case VMW_AND:
			fprintf(vmw->tar, "and\n");
			break;
		case VMW_OR:
			fprintf(vmw->tar, "or\n");
			break;
		case VMW_NOT:
			fprintf(vmw->tar, "not\n");
			break;
	}
}
void writeLabel(VMWriter *vmw, char *label){
	fprintf(vmw->tar, "label %s\n", label);	
}
void writeGoto(VMWriter *vmw, char *label){
	fprintf(vmw->tar, "goto %s\n", label);	
}
void writeIf(VMWriter *vmw, char *label){
	fprintf(vmw->tar, "if-goto %s\n", label);	
}
void writeCall(VMWriter *vmw, char *name, int nArgs){
	fprintf(vmw->tar, "call %s %d\n", name, nArgs);	
}
void writeFunction(VMWriter *vmw, char *name, int nLocals){
	fprintf(vmw->tar, "function %s %d\n", name, nLocals);
}
void writeReturn(VMWriter *vmw){
	fprintf(vmw->tar, "return\n");
}
void closeVMWriter(VMWriter *vmw){
	vmw->tar = NULL;
}