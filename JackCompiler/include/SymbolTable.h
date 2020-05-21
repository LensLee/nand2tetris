#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
/**/
typedef unsigned char Kind;
/**/
#define SYM_NONE 0
#define SYM_STATIC 1
#define SYM_FIELD 2
#define SYM_ARG 3
#define SYM_VAR 4
/**/
typedef struct{
	char *name;
	char *type;
	Kind kind;
	int idx;
} Symbol;

typedef struct{
	char *tab_name;//table name
	int tab_size;//table size
	int sym_count;//symbol count
	Symbol *rows;
} SymbolTable;

/*Creates a new symbol table.*/
void setSymbolTable(SymbolTable*);
void unsetSymbolTable(SymbolTable*);
/*Starts a new subroutine scope
(i.e., resets the subroutine's
symbol table).*/
void startSubroutine(SymbolTable*);
/*Defines a new identifier of the
given name, type, and kind,
and assigns it a running index.
STATIC and FIELD identifiers
have a class scope, while ARG
and VAR identifiers have a
subroutine scope.*/
void defineSymbol(SymbolTable*, char*, char*, Kind);
/*Returns the number of
variables of the given kind
already defined in the current
scope.*/
int getVarCount(SymbolTable*, Kind);
/*Returns the kind of the named
identifier in the current scope.
If the identifier is unknown in
the current scope, returns
NONE.*/
Kind getKind(SymbolTable*, char*);
/*Returns the type of the named
identifier in the current scope.*/
char* getType(SymbolTable*, char*);
/*Returns the index assigned to
the named identifier.*/
int getIndex(SymbolTable*, char*);

#endif