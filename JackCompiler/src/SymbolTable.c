#include "..\include\SymbolTable.h"
#include <stdlib.h>
#include <string.h>
#define TABSIZ 8
#define FREE(p) do{ free(p); p = NULL; }while(0)
void setSymbolTable(SymbolTable *symtab){
	symtab->tab_size = TABSIZ;
	symtab->sym_count = 0;
	symtab->tab_name = NULL;
	symtab->rows = malloc(sizeof(Symbol)*TABSIZ);
}
void unsetSymbolTable(SymbolTable *symtab){
	if(symtab->tab_name != NULL){
		FREE(symtab->tab_name);
	}
	if(symtab->sym_count > 0){
		for(int i=0; i<symtab->sym_count; i++){
			FREE(symtab->rows[i].name);
			FREE(symtab->rows[i].type);
		}
	}
}
void startSubroutine(SymbolTable *symtab){
	if(symtab->sym_count > 0){
		for(int i=0; i<symtab->sym_count; i++){
			FREE(symtab->rows[i].name);
			FREE(symtab->rows[i].type);
		}
		symtab->sym_count = 0;
		if(symtab->tab_name != NULL){
			FREE(symtab->tab_name);
		}
	}
}
void defineSymbol(SymbolTable *symtab, char *name, char *type, Kind kind){
	if(symtab->sym_count == symtab->tab_size){
		symtab->tab_size*=2;
		symtab->rows = realloc(symtab->rows, sizeof(Symbol)*symtab->tab_size);
	}
	Symbol* sym = &symtab->rows[symtab->sym_count];
	char** sym_n = &sym->name;
	char** sym_t = &sym->type;
	*sym_n = malloc(strlen(name)+1);
	*sym_t = malloc(strlen(type)+1);
	strcpy(*sym_n, name);
	strcpy(*sym_t, type);
	sym->kind = kind;
	sym->idx = getVarCount(symtab, kind);
	symtab->sym_count++;
}
int getVarCount(SymbolTable *symtab, Kind kind){
	int count = 0;
	for(int i=0; i<symtab->sym_count; i++){
		if(symtab->rows[i].kind == kind){
			count++;
		}
	}
	return count;
}
Kind getKind(SymbolTable *symtab, char *name){
	for(int i=0; i<symtab->sym_count; i++){
		if(strcmp(symtab->rows[i].name, name) == 0){
			return symtab->rows[i].kind;
		}
	}
	return SYM_NONE;
}
char* getType(SymbolTable *symtab, char *name){
	for(int i=0; i<symtab->sym_count; i++){
		if(strcmp(symtab->rows[i].name, name) == 0){
			return symtab->rows[i].type;
		}
	}
	return NULL;
}
int getIndex(SymbolTable *symtab, char *name){
	for(int i=0; i<symtab->sym_count; i++){
		if(strcmp(symtab->rows[i].name, name) == 0){
			return symtab->rows[i].idx;
		}
	}
	return -1;
}