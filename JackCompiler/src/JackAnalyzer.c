#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include "..\include\JackTokenizer.h"
#include "..\include\CompilationEngine.h"
// #include <unistd.h>//getcwd()
// #include <libgen.h>//dirname(), basename()
// #include <signal.h>

bool is_jackfile(const char *filename, const char *_postfix){
	char *postfix = strstr(filename, _postfix);
	if( postfix != NULL && strlen(postfix) == strlen(_postfix) ){
		return true;
	}
	return false;
}

char* getTargetFileName
(const char *src_name,
	const char *src_format,
	const char *tar_format){
	int file_name_len = strlen(src_name)-strlen(src_format);
	int tar_name_len = file_name_len+strlen(tar_format);
	char *tar_name = calloc(tar_name_len+1, sizeof(char));
	strncpy(tar_name, src_name, file_name_len);
	strcat(tar_name, tar_format);
	return tar_name;
}

int main(int argc, char *argv[]){
	if( argc < 3 ){
		if( argc == 1 ){
			DIR *dirp;
			struct dirent *dp;
			if( (dirp = opendir(".")) != NULL ){
				int file_success = 0;
				int file_count = 0;
				CompilationEngine cmp;
				while( (dp = readdir(dirp)) != NULL ){
					if( is_jackfile(dp->d_name, ".jack") ){
						char *src_name = dp->d_name;
						FILE *src = fopen(src_name, "r");
						if( src != NULL ){
							char *tar_name = getTargetFileName(src_name,
																									".jack", ".vm");
							FILE *tar = fopen(tar_name, "w");
							if( tar != NULL ){
								JackTokenizer jkt;
								setJackTokenizer(&jkt, src);
								setCompilationEngine(&cmp, &jkt, tar);
								bool _success = true;
								do{
									if(hasMoreTokens(&jkt)){
										advance(&jkt);
										if( getKeyword(&jkt) == JKT_CLASS ){
											compileClass(&cmp);
										}
										else{
											fprintf(stderr,
												"%s: Cannot find \"class\" keyword!\n",
												src_name);
											_success = false;
										}
									}
									else{
										fprintf(stderr,
											"%s: Cannot find valid token!\n",
											src_name);
										_success = false;
									}
								}while( 0 );
								unsetCompilationEngine(&cmp);
								unsetJackTokenizer(&jkt);
								if( !_success ){
									if( remove(tar_name) != 0 ){
										fprintf(stderr, "%s", tar_name);
										perror("remove");
									}
								}
								_success = true;
								if( fclose(src) != 0 ){
									fprintf(stderr, "%s", src_name);
									perror("fclose");
									_success = false;
								}
								if( fclose(tar) != 0 ){
									fprintf(stderr, "%s", tar_name);
									perror("fclose");
									_success = false;
								}
								if(_success){
									file_success++;
								}
								file_count++;							
							}
							else{
								if( fclose(src) != 0 ){
									fprintf(stderr, "%s", src_name);
									perror("fclose");
								}
								fprintf(stderr, "%s", tar_name);
								perror("fopen");
							}
							free(tar_name);
						}
						else{
							fprintf(stderr, "%s", src_name);
							perror("fopen");
						}
					}
				}
				if( closedir(dirp) != 0 ){
					perror("closedir");
				}
				if( file_count == 0 ){
					fprintf(stderr, "No jack file found!\n");
				}
				if( file_count > 0 ){
					fprintf(stdout, "Process %d file, ", file_count);
					fprintf(stdout, "%d file success!\n", file_success);
				}
			}
			else{
				perror("opendir");
				exit(1);
			}
		}
		else{
			if( is_jackfile(argv[1], ".jack") ){
				char *src_name = argv[1];
				FILE *src = fopen(src_name, "r");
				if( src != NULL ){
					char *tar_name = getTargetFileName(src_name, ".jack", ".vm");
					FILE *tar = fopen(tar_name, "w");
					if( tar != NULL ){
						CompilationEngine cmp;
						JackTokenizer jkt;
						setJackTokenizer(&jkt, src);
						setCompilationEngine(&cmp, &jkt, tar);
						bool _success = true;
						do{
							if( hasMoreTokens(&jkt) ){
								advance(&jkt);
								if( getKeyword(&jkt) == JKT_CLASS ){
									compileClass(&cmp);
								}
								else{
									fprintf(stderr,
										"%s: Cannot find \"class\" keyword!\n",
										src_name);
									_success = false;
								}
							}
							else{
								fprintf(stderr,
											"%s: Cannot find valid token!\n",
											src_name);
								_success = false;
							}
						}while( 0 );
						unsetCompilationEngine(&cmp);
						unsetJackTokenizer(&jkt);
						if( !_success ){
							if( remove(tar_name) != 0 ){
								fprintf(stderr, "%s", tar_name);
								perror("remove");
							}
						}
						_success = true;
						if( fclose(src) != 0 ){
							fprintf(stderr, "%s", src_name);
							perror("fclose");
							_success = false;
						}
						if( fclose(tar) != 0 ){
							fprintf(stderr, "%s", tar_name);
							perror("fclose");
							_success = false;
						}
					}
					else{
						fprintf(stderr, "%s", tar_name);
						perror("fopen");
					}
					free(tar_name);
				}
				else{
					fprintf(stderr, "%s", src_name);
					perror("fopen");
					exit(1);
				}
			}
			else{
				DIR *dirp;
				struct dirent *dp;
				if( (dirp = opendir(argv[1])) != NULL ){
					int file_success = 0;
					int file_count = 0;
					CompilationEngine cmp;
					while( (dp = readdir(dirp)) != NULL ){
						if( is_jackfile(dp->d_name, ".jack") ){
							int len = strlen(argv[1])+strlen(dp->d_name)+1;
							char *src_name = calloc(len+1, sizeof(char));
							strcpy(src_name, argv[1]);
							strcat(src_name, "\\");
							strcat(src_name, dp->d_name);
							FILE *src = fopen(src_name, "r");
							if( src != NULL ){
								char *tar_name = getTargetFileName(src_name,
																									".jack", ".vm");
								FILE *tar = fopen(tar_name, "w");
								if( tar != NULL ){
									JackTokenizer jkt;
									setJackTokenizer(&jkt, src);
									setCompilationEngine(&cmp, &jkt, tar);
									bool _success = true;
									do{
										if( hasMoreTokens(&jkt) ){
											advance(&jkt);
											if( getKeyword(&jkt) == JKT_CLASS ){
												compileClass(&cmp);
											}
											else{
												fprintf(stderr,
													"%s: Cannot find \"class\" keyword!\n",
													src_name);
												_success = false;
											}
										}
										else{
											fprintf(stderr,
												"%s: Cannot find valid token!\n",
												src_name);
											_success = false;
										}
									}while( 0 );
									unsetCompilationEngine(&cmp);
									unsetJackTokenizer(&jkt);
									if( !_success ){
										if( remove(tar_name) != 0 ){
											fprintf(stderr, "%s", tar_name);
											perror("remove");
										}
									}
									_success = true;
									if( fclose(src) != 0 ){
										fprintf(stderr, "%s", src_name);
										perror("fclose");
										_success = false;
									}
									if( fclose(tar) != 0 ){
										fprintf(stderr, "%s", tar_name);
										perror("fclose");
										_success = false;
									}
									if(_success){
										file_success++;
									}
									file_count++;
								}
								else{
									if( fclose(src) != 0 ){
										fprintf(stderr, "%s", src_name);
										perror("fclose");
									}
									fprintf(stderr, "%s", tar_name);
									perror("fopen");
								}
								free(tar_name);
							}
							else{
								fprintf(stderr, "%s", src_name);
								perror("fopen");
							}
							free(src_name);
						}
					}
					if( closedir(dirp) != 0 ){
						perror("closedir");
					}
					if( file_count == 0 ){
						fprintf(stderr, "No jack file found!\n");
					}
					if( file_count > 0 ){
						fprintf(stdout, "Process %d file, ", file_count);
						fprintf(stdout, "%d file success!\n", file_success);
					}
				}
				else{
					perror("opendir");
					exit(1);
				}
			}
		}
	}
	return 0;
}