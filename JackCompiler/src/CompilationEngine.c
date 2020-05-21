#include "..\include\CompilationEngine.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
/*
	(Software engineer perspective, not semantic requirement)
	Class
	1. Contains functions only
	2. (no fields, constructors, or methods)
	3. Offers a "library of services"
*/
#define FREE(p) do{ free(p); p = NULL; }while(0)

#define jkt_adv() CompilationEngine_advance_JackTokenizer(cmp)
bool CompilationEngine_advance_JackTokenizer(CompilationEngine *cmp){
	if(hasMoreTokens(cmp->src)){
		advance(cmp->src);
		return true;
	}
	else{
		return false;
	}
}

#define cmp_print_otag(open_tag) fprintf(cmp->tar, "<%s>\n", open_tag)
#define cmp_print_ctag(close_tag) fprintf(cmp->tar, "</%s>\n", close_tag)

void printToken(CompilationEngine *cmp){
	JackTokenizer *jkt = cmp->src;
	FILE *output = cmp->tar;
	TokenType tok_t = getTokenType(jkt);
	switch(tok_t){
		case JKT_KEYWORD:
			fprintf(output, "<%s> %s </%s>\n",
				token_types[tok_t],
				keywords[getKeyword(jkt)],
				token_types[tok_t]);
			break;
		case JKT_SYMBOL:
			fprintf(output, "<%s> ", token_types[tok_t]);
			switch(getSymbol(jkt)){
				case '<': fprintf(output, "%s", "&lt;"); break;
				case '>': fprintf(output, "%s", "&gt;"); break;
				case '\"': fprintf(output, "%s", "&quot;"); break;
				case '&': fprintf(output, "%s", "&amp;"); break;
				default : fprintf(output, "%c", getSymbol(jkt)); break;
			}
			fprintf(output, " </%s>\n", token_types[tok_t]);
			break;
		case JKT_IDENTIFIER:
			fprintf(output, "<%s> %s </%s>\n",
				token_types[tok_t],
				getIdentifier(jkt),
				token_types[tok_t]);
			break;
		case JKT_INT_CONST:
			fprintf(output, "<%s> %d </%s>\n",
				token_types[tok_t],
				getIntVal(jkt),
				token_types[tok_t]);
			break;
		case JKT_STRING_CONST:
			fprintf(output, "<%s> %s </%s>\n",
				token_types[tok_t],
				getStringVal(jkt),
				token_types[tok_t]);
			break;
	}
}

#define cmp_stack_push(cmd) CompilationEngine_stack_push(cmp, cmd)
#define cmp_stack_pop() CompilationEngine_stack_pop(cmp)

void CompilationEngine_stack_push(CompilationEngine *cmp, command cmd){
	if(cmp->stack_top<256){
		cmp->op_stack[cmp->stack_top] = cmd;
		cmp->stack_top++;
	}
}
command CompilationEngine_stack_pop(CompilationEngine *cmp){
	if(cmp->stack_top>0){
		cmp->stack_top--;
		return cmp->op_stack[cmp->stack_top];
	}
	return 0;
}

void writeCommand(VMWriter *vmwriter, command cmd){
	switch(cmd){
		case VMW_MULT:
			writeCall(vmwriter, "Math.multiply", 2);
			break;
		case VMW_DIV:
			writeCall(vmwriter, "Math.divide", 2);
			break;	
		case VMW_ADD:
		case VMW_SUB: 
		case VMW_NEG:
		case VMW_EQ: 
		case VMW_GT:
		case VMW_LT: 
		case VMW_AND:
		case VMW_OR: 
		case VMW_NOT:
			writeArithmetic(vmwriter, cmd);
			break;
	}
}
void setCompilationEngine(CompilationEngine *cmp, JackTokenizer *source, FILE *target){
	cmp->src = source;
	cmp->tar = stdout;
	setVMWriter(&cmp->vmw, target);
	setSymbolTable(&cmp->symtab[0]);//store class's symbol table
	setSymbolTable(&cmp->symtab[1]);//store subroutine's symbol table
	cmp->stack_top = 0;
	cmp->if_label_c = 0;
	cmp->while_label_c = 0;
}
void unsetCompilationEngine(CompilationEngine *cmp){
	closeVMWriter(&cmp->vmw);
	unsetSymbolTable(&cmp->symtab[0]);
	unsetSymbolTable(&cmp->symtab[1]);
}
void compileClass(CompilationEngine *cmp){
	// cmp_print_otag("class");
	// printToken(cmp);//class
	SymbolTable *classSymbolTbale = &cmp->symtab[0];
	startSubroutine(classSymbolTbale);
	if(jkt_adv()){
		JackTokenizer *jkt = cmp->src;
		if(getTokenType(jkt) == JKT_IDENTIFIER){
			// printToken(cmp);//className
			classSymbolTbale->tab_name = strdup(getIdentifier(jkt));
			if(jkt_adv()){
				if(getSymbol(jkt) == '{'){
					// printToken(cmp);//{
					bool _continue = true;
					do{
						if(jkt_adv()){
							Keyword keyword = getKeyword(jkt);
							if(keyword == JKT_STATIC || keyword == JKT_FIELD){
								compileClassVarDec(cmp);
							}
							else if(keyword >= JKT_METHOD &&
									keyword <= JKT_CONSTRUCTOR){
								compileSubroutineDec(cmp);
							}
							else{
								_continue = false;
							}
						}
						else{
							_continue = false;
						}
					}while(_continue);

					// if(getSymbol(jkt) == '}'){
					// 	printToken(cmp);//}
					// }
				}
			}
		}
	}
	// cmp_print_ctag("class");
}
void compileClassVarDec(CompilationEngine* cmp){
	// cmp_print_otag("classVarDec");
	// printToken(cmp);//static|field
	JackTokenizer *jkt = cmp->src;
	Keyword keyword = getKeyword(jkt);
	Kind kind;
	switch(keyword){
		case JKT_STATIC:
			kind = SYM_STATIC;
			break;
		case JKT_FIELD:
			kind = SYM_FIELD;
			break;
	}
	if(jkt_adv()){
		keyword = getKeyword(jkt);
		if((keyword >= JKT_INT && keyword <= JKT_CHAR) ||
			getTokenType(jkt) == JKT_IDENTIFIER){
			// printToken(cmp);//className|int|char|boolean
			char *type;
			switch(getTokenType(jkt)){
				case JKT_KEYWORD:
					type = strdup(keywords[getKeyword(jkt)]);
					break;
				case JKT_IDENTIFIER:
					type = strdup(getIdentifier(jkt));
					break;
			}
			if(jkt_adv()){
				if(getTokenType(jkt) == JKT_IDENTIFIER){
					// printToken(cmp);//varName
					SymbolTable *classSymbolTbale = &cmp->symtab[0];
					defineSymbol(classSymbolTbale, getIdentifier(jkt), 
												type, kind);
					bool _continue = true;
					do{
						if(jkt_adv()){
							if(getSymbol(jkt) == ','){
								// printToken(cmp);//,
								if(jkt_adv()){
									if(getTokenType(jkt) == JKT_IDENTIFIER){
										// printToken(cmp);//varName
										defineSymbol(classSymbolTbale, getIdentifier(jkt),
																type, kind);
									}
									else{
										_continue = false;
									}
								}
								else{
									_continue = false;
								}
							}
							else{
								_continue = false;
							}
						}
						else{
							_continue = false;
						}
					}while(_continue);
					// if(getSymbol(jkt) == ';'){
					// 	printToken(cmp);//;
					// }
				}
			}
			free(type);
		}
	}
	// cmp_print_ctag("classVarDec");
}
void compileSubroutineDec(CompilationEngine* cmp){
	// cmp_print_otag("subroutineDec");
	// printToken(cmp);//constructor|function|method
	SymbolTable *subroutineSymbolTbale = &cmp->symtab[1];
	startSubroutine(subroutineSymbolTbale);
	cmp->stack_top = 0;
	cmp->if_label_c = 0;
	cmp->while_label_c = 0;
	
	JackTokenizer *jkt = cmp->src;
	Keyword keyword = getKeyword(jkt);
	cmp->func_t = keyword;
	if(keyword == JKT_METHOD){
		defineSymbol(subroutineSymbolTbale, "this", "", SYM_ARG);
	}
	if(jkt_adv()){
		Keyword keyword = getKeyword(jkt);
		if((keyword >= JKT_INT && keyword <= JKT_VOID) ||
			getTokenType(jkt) == JKT_IDENTIFIER){
			// printToken(cmp);//className|void|int|char|boolean
			if(jkt_adv()){
				if(getTokenType(jkt) == JKT_IDENTIFIER){
					// printToken(cmp);//subroutineName
					char *classSymbolTbale_name = cmp->symtab[0].tab_name;
					char *identifier = getIdentifier(jkt);
					int len = strlen(classSymbolTbale_name)+strlen(identifier)+2;
					char *subroutineSymbolTbale_name = subroutineSymbolTbale->tab_name;
					subroutineSymbolTbale_name = realloc(subroutineSymbolTbale_name, len);
					strcpy(subroutineSymbolTbale_name, classSymbolTbale_name);
					strcat(subroutineSymbolTbale_name, ".");
					strcat(subroutineSymbolTbale_name, identifier);
					subroutineSymbolTbale->tab_name = subroutineSymbolTbale_name;
					if(jkt_adv()){
						if(getSymbol(jkt) == '('){
							// printToken(cmp);//(
							if(jkt_adv()){
								compileParameterList(cmp);
								if(getSymbol(jkt) == ')'){
									// printToken(cmp);//)
									if(jkt_adv()){
										if(getSymbol(jkt) == '{'){
											compileSubroutineBody(cmp);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	// cmp_print_ctag("subroutineDec");
}
void compileParameterList(CompilationEngine* cmp){
	// cmp_print_otag("parameterList");
	JackTokenizer *jkt = cmp->src;
	Keyword keyword = getKeyword(jkt);
	if((keyword >= JKT_INT && keyword <= JKT_CHAR) ||
		getTokenType(jkt) == JKT_IDENTIFIER){
		// printToken(cmp);//className|int|char|boolean
		TokenType tok_t = getTokenType(jkt);
		char *type;
		switch(tok_t){
			case JKT_KEYWORD:
				type = strdup(keywords[keyword]);
				break;
			case JKT_IDENTIFIER:
				type = strdup(getIdentifier(jkt));
				break;
		}
		if(jkt_adv()){
			if(getTokenType(jkt) == JKT_IDENTIFIER){
				// printToken(cmp);//varName
				SymbolTable *subroutineSymbolTbale = &cmp->symtab[1];
				defineSymbol(subroutineSymbolTbale,
					getIdentifier(jkt), type, SYM_ARG);
				bool _continue = true;
				do{
					if(jkt_adv()){
						if(getSymbol(jkt) == ','){
							// printToken(cmp);//,
							if(jkt_adv()){
								keyword = getKeyword(jkt);
								tok_t = getTokenType(jkt);
								if((keyword >= JKT_INT && keyword <= JKT_CHAR) ||
									tok_t == JKT_IDENTIFIER){
									// printToken(cmp);//className|int|char|boolean
									int len;
									switch(tok_t){
										case JKT_KEYWORD:
											len = strlen(keywords[keyword])+1;
											type = realloc(type, len);
											strcpy(type, keywords[keyword]); 
											break;
										case JKT_IDENTIFIER:
											len = strlen(getIdentifier(jkt))+1;
											type = realloc(type, len);
											strcpy(type, getIdentifier(jkt));
											break;
									}
									if(jkt_adv()){
										if(getTokenType(jkt) == JKT_IDENTIFIER){
											// printToken(cmp);//varName
											defineSymbol(subroutineSymbolTbale,
												getIdentifier(jkt), type, SYM_ARG);
										}
										else{
											_continue = false;
										}
									}
									else{
										_continue = false;
									}
								}
							}
							else{
								_continue = false;
							}
						}
						else{
							_continue = false;
						}
					}
					else{
						_continue = false;
					}
				}while(_continue);
			}
		}
		free(type);
	}
	// cmp_print_ctag("parameterList");
}
void compileSubroutineBody(CompilationEngine* cmp){
	// cmp_print_otag("subroutineBody");
	// printToken(cmp);//{
	JackTokenizer *jkt = cmp->src;
	bool _continue = true;
	do{
		if(jkt_adv()){
			if(getKeyword(jkt) == JKT_VAR){
				compileVarDec(cmp);
			}
			else{
				_continue = false;
			}
		}
		else{
			_continue = false;
		}
	}while(_continue);
	VMWriter *vmwriter = &cmp->vmw;
	SymbolTable *classSymbolTbale = &cmp->symtab[0];
	SymbolTable *subroutineSymbolTbale = &cmp->symtab[1];
	char *subroutineSymbolTbale_name = subroutineSymbolTbale->tab_name;
	writeFunction(vmwriter, subroutineSymbolTbale_name,
								getVarCount(subroutineSymbolTbale, SYM_VAR));
	Keyword function_type = cmp->func_t;
	switch(function_type){
		case JKT_METHOD:
			writePush(vmwriter, VMW_ARG, 0);
			writePop(vmwriter, VMW_POINTER, 0);
			break;
		case JKT_CONSTRUCTOR:
			writePush(vmwriter, VMW_CONST,
								getVarCount(classSymbolTbale, SYM_FIELD));
			writeCall(vmwriter, "Memory.alloc", 1);
			writePop(vmwriter, VMW_POINTER, 0);
			break;
	}
	compileStatements(cmp);
	// if(getSymbol(jkt) == '}'){
	// 	printToken(cmp);//}
	// }
	// cmp_print_ctag("subroutineBody");
}
void compileVarDec(CompilationEngine* cmp){
	// cmp_print_otag("varDec");
	// printToken(cmp);//var
	if(jkt_adv()){
		JackTokenizer *jkt = cmp->src;
		Keyword keyword = getKeyword(jkt);
		TokenType tok_t = getTokenType(jkt);
		if((keyword >= JKT_INT && keyword <= JKT_CHAR) ||
					tok_t == JKT_IDENTIFIER){
			// printToken(cmp);//className|int|char|boolean
			char* type;
			switch(tok_t){
				case JKT_KEYWORD:
					type = strdup(keywords[keyword]);
					break;
				case JKT_IDENTIFIER:
					type = strdup(getIdentifier(jkt));
					break;
			}
			if(jkt_adv()){
				if(getTokenType(jkt) == JKT_IDENTIFIER){
					// printToken(cmp);//varName
					SymbolTable *subroutineSymbolTbale = &cmp->symtab[1];
					defineSymbol(subroutineSymbolTbale,
											getIdentifier(jkt), type, SYM_VAR);
					bool _continue = true;
					do{
						if(jkt_adv()){
							if(getSymbol(jkt) == ','){
								// printToken(cmp);//,
								if(jkt_adv()){
									if(getTokenType(jkt) == JKT_IDENTIFIER){
										// printToken(cmp);//varName
										defineSymbol(subroutineSymbolTbale,
																getIdentifier(jkt), type, SYM_VAR);
									}
									else{
										_continue = false;
									}
								}
								else{
									_continue = false;
								}
							}
							else{
								_continue = false;
							}
						}
						else{
							_continue = false;
						}
					}while(_continue);
					// if(getSymbol(jkt) == ';'){
					// 	printToken(cmp);//;
					// }
				}
			}
			free(type);
		}
	}
	// cmp_print_ctag("varDec");
}
void compileStatements(CompilationEngine* cmp){
	// cmp_print_otag("statements");
	JackTokenizer *jkt = cmp->src;
	Keyword keyword = getKeyword(jkt);
	if(keyword >= JKT_LET && keyword <= JKT_RETURN){
		bool _continue = true;
		while(_continue){
			switch(getKeyword(jkt)){
				case JKT_LET:
					compileLet(cmp);
					break;
				case JKT_DO:
					compileDo(cmp);
					break;
				case JKT_IF:
					compileIf(cmp);
					break;
				case JKT_WHILE:
					compileWhile(cmp);
					break;
				case JKT_RETURN:
					compileReturn(cmp);
					break;
				default:
					_continue = false;
					break;
			}
		}
	}
	// cmp_print_ctag("statements");
}
void compileLet(CompilationEngine* cmp){
	// cmp_print_otag("letStatement");
	// printToken(cmp);//let
	if(jkt_adv()){
		JackTokenizer *jkt = cmp->src;
		if(getTokenType(jkt) == JKT_IDENTIFIER){
			// printToken(cmp);//varName
			char *varName = strdup(getIdentifier(jkt));
			if(jkt_adv()){
				SymbolTable *classSymbolTbale = &cmp->symtab[0];
				SymbolTable *subroutineSymbolTbale = &cmp->symtab[1];
				Kind kind = getKind(subroutineSymbolTbale, varName);
				int idx;
				if(kind != SYM_NONE){
					idx = getIndex(subroutineSymbolTbale, varName);
				}
				else{
					kind = getKind(classSymbolTbale, varName);
					if(kind != SYM_NONE){
						idx = getIndex(classSymbolTbale, varName);
					}
				}
				segment seg = 0;
				switch(kind){
					case SYM_STATIC:
						seg = VMW_STATIC;
						break;
					case SYM_FIELD:
						seg = VMW_THIS;
						break;
					case SYM_ARG:
						seg = VMW_ARG;
						break;
					case SYM_VAR:
						seg = VMW_LOCAL;
						break;
				}
				VMWriter *vmwriter = &cmp->vmw;
				if(getSymbol(jkt) == '['){
					// printToken(cmp);//[
					if(jkt_adv()){
						compileExpression(cmp);
						writePush(vmwriter, seg, idx);
						writeArithmetic(vmwriter, VMW_ADD);
						if(getSymbol(jkt) == ']'){
							// printToken(cmp);//]
							if(jkt_adv()){
								if(getSymbol(jkt) == '='){
									// printToken(cmp);//=
									if(jkt_adv()){
										compileExpression(cmp);
										writePop(vmwriter, VMW_TEMP, 0);
										writePop(vmwriter, VMW_POINTER, 1);
										writePush(vmwriter, VMW_TEMP, 0);
										writePop(vmwriter, VMW_THAT, 0);
										if(getSymbol(jkt) == ';'){
											// printToken(cmp);//;
											jkt_adv();
										}
									}
								}
							}
						}
					}
				}
				else{
					// printToken(cmp);//=
					if(jkt_adv()){
						compileExpression(cmp);
						writePop(vmwriter, seg, idx);
						if(getSymbol(jkt) == ';'){
							// printToken(cmp);//;
							jkt_adv();
						}
					}
				}
			}
			free(varName);
		}
	}
	// cmp_print_ctag("letStatement");
}
void compileIf(CompilationEngine* cmp){
	// cmp_print_otag("ifStatement");
	// printToken(cmp);//if
	unsigned short label_idx = cmp->if_label_c++;
	int digit_len;
	if(label_idx == 0){
		digit_len = 1;
	}
	else{
		digit_len = floor(log10(label_idx))+1;
	}
	char label_true[strlen("IF_TRUE")+digit_len+1];
	sprintf(label_true, "%s%u", "IF_TRUE", label_idx);

	char label_false[strlen("IF_FALSE")+digit_len+1];
	sprintf(label_false, "%s%u", "IF_FALSE", label_idx);

	if(jkt_adv()){
		JackTokenizer *jkt = cmp->src;
		if(getSymbol(jkt) == '('){
			// printToken(cmp);//(
			if(jkt_adv()){
				compileExpression(cmp);
				if(getSymbol(jkt) == ')'){
					// printToken(cmp);//)
					VMWriter *vmwriter = &cmp->vmw;
					writeIf(vmwriter, label_true);
					writeGoto(vmwriter, label_false);
					writeLabel(vmwriter, label_true);
					if(jkt_adv()){
						if(getSymbol(jkt) == '{'){
							// printToken(cmp);//{
							if(jkt_adv()){
								compileStatements(cmp);
								if(getSymbol(jkt) == '}'){
									// printToken(cmp);//}
									if(jkt_adv()){
										if(getKeyword(jkt) == JKT_ELSE){
											// printToken(cmp);//else
											char label_end[strlen("IF_END")+digit_len+1];
											sprintf(label_end, "%s%u", "IF_END", label_idx);
											writeGoto(vmwriter, label_end);
											writeLabel(vmwriter, label_false);
											if(jkt_adv()){
												if(getSymbol(jkt) == '{'){
													// printToken(cmp);//{
													if(jkt_adv()){
														compileStatements(cmp);
														if(getSymbol(jkt) == '}'){
															// printToken(cmp);//}
															writeLabel(vmwriter, label_end);
															jkt_adv();
														}
													}
												}
											}
										}
										else{
											writeLabel(vmwriter, label_false);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	// cmp_print_ctag("ifStatement");
}
void compileWhile(CompilationEngine* cmp){
	// cmp_print_otag("whileStatement");
	// printToken(cmp);//while
	unsigned short label_idx = cmp->while_label_c++;
	int digit_len;
	if(label_idx == 0){
		digit_len = 1;
	}
	else{
		digit_len = floor(log10(label_idx))+1;
	}

	char label_exp[strlen("WHILE_EXP")+digit_len+1];
	sprintf(label_exp, "%s%u", "WHILE_EXP", label_idx);

	char label_end[strlen("WHILE_END")+digit_len+1];
	sprintf(label_end, "%s%u", "WHILE_END", label_idx);

	VMWriter *vmwriter = &cmp->vmw;
	writeLabel(vmwriter, label_exp);

	if(jkt_adv()){
		JackTokenizer *jkt = cmp->src;
		if(getSymbol(jkt) == '('){
			// printToken(cmp);//(
			if(jkt_adv()){
				compileExpression(cmp);
				writeArithmetic(vmwriter, VMW_NOT);
				if(getSymbol(jkt) == ')'){
					// printToken(cmp);//)
					writeIf(vmwriter, label_end);
					if(jkt_adv()){
						if(getSymbol(jkt) == '{'){
							// printToken(cmp);//{
							if(jkt_adv()){
								compileStatements(cmp);
								if(getSymbol(jkt) == '}'){
									// printToken(cmp);//}
									writeGoto(vmwriter, label_exp);
									writeLabel(vmwriter, label_end);
									jkt_adv();
								}
							}
						}
					}
				}
			}
		}
	}
	// cmp_print_ctag("whileStatement");
}
void compileDo(CompilationEngine* cmp){
	// cmp_print_otag("doStatement");
	// printToken(cmp);//do
	if(jkt_adv()){
		JackTokenizer *jkt = cmp->src;
		if(getTokenType(jkt) == JKT_IDENTIFIER){
			SymbolTable *classSymbolTbale = &cmp->symtab[0];
			SymbolTable *subroutineSymbolTbale = &cmp->symtab[1];
			char* identifier = strdup(getIdentifier(jkt));
			Kind kind = getKind(subroutineSymbolTbale, identifier);
			int idx;
			char* type;
			if(kind != SYM_NONE){
				idx = getIndex(subroutineSymbolTbale, identifier);
				type = getType(subroutineSymbolTbale, identifier);
			}
			else{
				kind = getKind(classSymbolTbale, identifier);
				if(kind != SYM_NONE){
					idx = getIndex(classSymbolTbale, identifier);
					type = getType(classSymbolTbale, identifier);
				}
			}
			segment seg = 0;
			switch(kind){
				case SYM_STATIC:
					seg = VMW_STATIC;
					break;
				case SYM_FIELD:
					seg = VMW_THIS;
					break;
				case SYM_ARG:
					seg = VMW_ARG;
					break;
				case SYM_VAR:
					seg = VMW_LOCAL;
					break;
			}
			// printToken(cmp);//subroutineName|className|varName
			if(jkt_adv()){
				VMWriter *vmwriter = &cmp->vmw;
				if(getSymbol(jkt) == '('){
					// printToken(cmp);//(
					if(jkt_adv()){
						char *classSymbolTbale_name = classSymbolTbale->tab_name;
						int len = strlen(classSymbolTbale_name)+strlen(identifier)+2;
						char* subroutineName = calloc(len, sizeof(char));
						strcpy(subroutineName, classSymbolTbale_name);
						strcat(subroutineName, ".");
						strcat(subroutineName, identifier);
						writePush(vmwriter, VMW_POINTER, 0);
						int nArgs = compileExpressionList(cmp)+1;
						if(getSymbol(jkt) == ')'){
							// printToken(cmp);//)
							writeCall(vmwriter, subroutineName, nArgs);
							writePop(vmwriter, VMW_TEMP, 0);
						}
						free(subroutineName);
					}
				}
				else if(getSymbol(jkt) == '.'){
					// printToken(cmp);//.					
					if(jkt_adv()){
						if(getTokenType(jkt) == JKT_IDENTIFIER){
							// printToken(cmp);//subroutineName
							int len = strlen(getIdentifier(jkt))+2;
							char* subroutineName;
							int nArgs;
							if(seg == 0){//className.subroutineName
								len += strlen(identifier);
								subroutineName = calloc(len, sizeof(char));
								strcpy(subroutineName, identifier);
								nArgs = 0;
							}
							else{
								len += strlen(type);
								subroutineName = calloc(len, sizeof(char));
								strcpy(subroutineName, type);
								writePush(vmwriter, seg, idx);
								nArgs = 1;
							}
							strcat(subroutineName, ".");
							strcat(subroutineName, getIdentifier(jkt));
							if(jkt_adv()){
								if(getSymbol(jkt) == '('){
									// printToken(cmp);//(
									if(jkt_adv()){
										nArgs += compileExpressionList(cmp);
										if(getSymbol(jkt) == ')'){
											// printToken(cmp);//)
											writeCall(vmwriter, subroutineName, nArgs);
											writePop(vmwriter, VMW_TEMP, 0);
										}
									}
								}
							}
							free(subroutineName);
						}
					}
				}
				if(jkt_adv()){
					if(getSymbol(jkt) == ';'){
						// printToken(cmp);
						jkt_adv();
					}
				}
			}
			free(identifier);
		}
	}
	// cmp_print_ctag("doStatement");
}
void compileReturn(CompilationEngine* cmp){
	// cmp_print_otag("returnStatement");
	// printToken(cmp);//return
	if(jkt_adv()){
		JackTokenizer *jkt = cmp->src;
		VMWriter *vmwriter = &cmp->vmw;
		if(getSymbol(jkt) == ';'){
			writePush(vmwriter, VMW_CONST, 0);
		}
		compileExpression(cmp);
		writeReturn(vmwriter);
		if(getSymbol(jkt) == ';'){
			// printToken(cmp);
			jkt_adv();
		}
	}
	// cmp_print_ctag("returnStatement");
}
void compileExpression(CompilationEngine* cmp){
	JackTokenizer *jkt = cmp->src;
	VMWriter *vmwriter = &cmp->vmw;
	TokenType tok_t = getTokenType(jkt);
	Keyword keyword = getKeyword(jkt);
	char symbol = getSymbol(jkt);
	if((tok_t >= JKT_IDENTIFIER && tok_t <= JKT_STRING_CONST) ||
		(keyword >= JKT_TRUE && keyword <= JKT_THIS) ||
		symbol == '(' || symbol == '-' || symbol == '~'){
		// cmp_print_otag("expression");
		compileTerm(cmp);
		symbol = getSymbol(jkt);
		while(symbol == '+' || symbol == '-' || symbol == '*' ||
			symbol == '/' || symbol == '&' || symbol == '|' ||
			symbol == '<' || symbol == '>' || symbol == '='){
			command cmd = 0;
			switch(symbol){
				case '+':
					cmd = VMW_ADD;
					break;
				case '-':
					cmd = VMW_SUB;
					break;
				case '*':
					cmd = VMW_MULT;
					break;
				case '/':
					cmd = VMW_DIV;
					break;
				case '&':
					cmd = VMW_AND;
					break;
				case '|':
					cmd = VMW_OR;
					break;
				case '<':
					cmd = VMW_LT;
					break;
				case '>':
					cmd = VMW_GT;
					break;
				case '=':
					cmd = VMW_EQ;
					break;
			}
			// printToken(cmp);//op
			if(jkt_adv()){
				tok_t = getTokenType(jkt);
				keyword = getKeyword(jkt);
				symbol = getSymbol(jkt);
				if((tok_t >= JKT_IDENTIFIER && tok_t <= JKT_STRING_CONST) ||
					(keyword >= JKT_TRUE && keyword <= JKT_THIS) ||
					symbol == '(' || symbol == '-' || symbol == '~'){
					bool postOrder = false;
					if(symbol == '('){
						postOrder = true;
					}
					else{
						cmp_stack_push(cmd);
					}
					compileTerm(cmp);
					if(postOrder){
						writeCommand(vmwriter, cmd);
					}
				}				
			}
			symbol = getSymbol(jkt);
		}
		// cmp_print_ctag("expression");
	}
}
void compileTerm(CompilationEngine* cmp){
	// cmp_print_otag("term");
	JackTokenizer *jkt = cmp->src;
	VMWriter *vmwriter = &cmp->vmw;
	TokenType tok_t = getTokenType(jkt);
	Keyword keyword = getKeyword(jkt);
	char symbol = getSymbol(jkt);
	if(tok_t == JKT_INT_CONST){
		// printToken(cmp);//integerConstant
		writePush(vmwriter, VMW_CONST, getIntVal(jkt));
		writeCommand(vmwriter, cmp_stack_pop());
		jkt_adv();
	}
	else if(tok_t == JKT_STRING_CONST){
		// printToken(cmp);//stringConstant
		char *string = getStringVal(jkt);
		int len = strlen(string);
		writePush(vmwriter, VMW_CONST, len);
		writeCall(vmwriter, "String.new", 1);
		for(int i=0; i<len; i++){
			writePush(vmwriter, VMW_CONST, (int)string[i]);
			writeCall(vmwriter, "String.appendChar", 2);
		}
		jkt_adv();
	}
	else if(keyword >= JKT_TRUE && keyword <= JKT_THIS){
		// printToken(cmp);//keywordConstant
		switch(keyword){
			case JKT_TRUE:
				writePush(vmwriter, VMW_CONST, 0);
				writeArithmetic(vmwriter, VMW_NOT);
				break;
			case JKT_FALSE:
				writePush(vmwriter, VMW_CONST, 0);
				break;
			case JKT_NULL:
				writePush(vmwriter, VMW_CONST, 0);
				break;
			case JKT_THIS:
				writePush(vmwriter, VMW_POINTER, 0);
				break;
		}
		writeCommand(vmwriter, cmp_stack_pop());
		jkt_adv();
	}
	else if(tok_t == JKT_IDENTIFIER){
		// printToken(cmp);
		char *identifier = strdup(getIdentifier(jkt));
		SymbolTable *classSymbolTbale = &cmp->symtab[0];
		SymbolTable *subroutineSymbolTbale = &cmp->symtab[1];
		Kind kind = getKind(subroutineSymbolTbale, identifier);
		int idx;
		char *type;
		if(kind != SYM_NONE){
			idx = getIndex(subroutineSymbolTbale, identifier);
			type = getType(subroutineSymbolTbale, identifier);
		}
		else{
			kind = getKind(classSymbolTbale, identifier);
			if(kind != SYM_NONE){
				idx = getIndex(classSymbolTbale, identifier);
				type = getType(classSymbolTbale, identifier);
			}
		}
		segment seg = 0;
		switch(kind){
			case SYM_STATIC:
				seg = VMW_STATIC;
				break;
			case SYM_FIELD:
				seg = VMW_THIS;
				break;
			case SYM_ARG:
				seg = VMW_ARG;
				break;
			case SYM_VAR:
				seg = VMW_LOCAL;
				break;
		}
		command cmd = cmp_stack_pop();
		if(jkt_adv()){
			symbol = getSymbol(jkt);
			if(symbol == '['){
				// printToken(cmp);//[
				if(jkt_adv()){
					compileExpression(cmp);
					writePush(vmwriter, seg, idx);
					writeArithmetic(vmwriter, VMW_ADD);
					if(getSymbol(jkt) == ']'){
						// printToken(cmp);//]
						writePop(vmwriter, VMW_POINTER, 1);
						writePush(vmwriter, VMW_THAT, 0);
						writeCommand(vmwriter, cmd);
						jkt_adv();
					}
				}
			}
			else if(symbol == '('){
				// printToken(cmp);//(
				char *classSymbolTbale_name = classSymbolTbale->tab_name;
				int len = strlen(classSymbolTbale_name)+strlen(identifier)+2;
				char* subroutineName = calloc(len, sizeof(char));
				strcpy(subroutineName, classSymbolTbale_name);
				strcat(subroutineName, ".");
				strcat(subroutineName, identifier);
				writePush(vmwriter, VMW_POINTER, 0);
				int nArgs = 1;
				if(jkt_adv()){
					nArgs += compileExpressionList(cmp);
					if(getSymbol(jkt) == ')'){
						// printToken(cmp);//)
						writeCall(vmwriter, subroutineName, nArgs);
						writeCommand(vmwriter, cmd);
						jkt_adv();
					}
				}
				free(subroutineName);
			}
			else if(symbol == '.'){
				// printToken(cmp);//.
				if(jkt_adv()){
					if(getTokenType(jkt) == JKT_IDENTIFIER){
						// printToken(cmp);//subroutineName
						int len = strlen(getIdentifier(jkt))+2;
						char* subroutineName;
						int nArgs;
						if(seg != 0){
							len += strlen(type);
							subroutineName = calloc(len, sizeof(char));
							strcpy(subroutineName, type);
							writePush(vmwriter, seg, idx);
							nArgs = 1;
						}
						else{
							len += strlen(identifier);
							subroutineName = calloc(len, sizeof(char));
							strcpy(subroutineName, identifier);
							nArgs = 0;	
						}
						strcat(subroutineName, ".");
						strcat(subroutineName, getIdentifier(jkt));

						if(jkt_adv()){
							if(getSymbol(jkt) == '('){
								// printToken(cmp);//(
								if(jkt_adv()){
									nArgs += compileExpressionList(cmp);
									if(getSymbol(jkt) == ')'){
										// printToken(cmp);//)
										writeCall(vmwriter, subroutineName, nArgs);
										writeCommand(vmwriter, cmd);
										jkt_adv();
									}
								}
							}
						}
						free(subroutineName);
					}
				}
			}
			else{
				writePush(vmwriter, seg, idx);
				writeCommand(vmwriter, cmd);
			}
		}
		free(identifier);
	}
	else if(symbol == '('){
		// printToken(cmp);//(
		if(jkt_adv()){
			compileExpression(cmp);
			if(getSymbol(jkt) == ')'){
				// printToken(cmp);//)
				jkt_adv();
			}
		}
	}
	else if(symbol == '-' || symbol == '~'){		
		// printToken(cmp);//-|~
		command cmd = 0;
		switch(symbol){
			case '-':
				cmd = VMW_NEG;
				break;
			case '~':
				cmd = VMW_NOT;
				break;
		}
		if(jkt_adv()){
			tok_t = getTokenType(jkt);
			keyword = getKeyword(jkt);
			symbol = getSymbol(jkt);
			if((tok_t >= JKT_IDENTIFIER && tok_t <= JKT_STRING_CONST) ||
				(keyword >= JKT_TRUE && keyword <= JKT_THIS) ||
				symbol == '(' || symbol == '-' || symbol == '~'){
				bool postOrder = false;
				if(symbol == '('){
					postOrder = true;
				}
				else{
					cmp_stack_push(cmd);
				}
				compileTerm(cmp);
				if(postOrder){
					writeCommand(vmwriter, cmd);
				}
			}
		}
	}
	// cmp_print_ctag("term");
}
int compileExpressionList(CompilationEngine* cmp){
	// cmp_print_otag("expressionList");
	int args = 0;
	JackTokenizer *jkt = cmp->src;
	if(getSymbol(jkt) != ')'){
		args++;
	}
	compileExpression(cmp);
	bool _continue = true;
	do{
		if(getSymbol(jkt) == ','){
			// printToken(cmp);//,
			if(jkt_adv()){
				if(getSymbol(jkt) != ')'){
					args++;
				}
				compileExpression(cmp);
			}
			else{
				_continue = false;
			}
		}
		else{
			_continue = false;
		}
	}while(_continue);
	// cmp_print_ctag("expressionList");
	return args;
}
