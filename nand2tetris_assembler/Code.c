#include <string.h>
/*Code: translates each field into its corresponding binary value*/
/*Returns the binary code of the dest mnemonic.*/
static char dest[4]={'\0'};
static char comp[8]={'\0'};
static char jump[4]={'\0'};
char* code_dest(char* mnemonic){
	int d1=0,d2=0,d3=0;
	if(mnemonic!=NULL){
		if(strchr(mnemonic,'M'))
			d3++;
		if(strchr(mnemonic,'D'))
			d2++;
		if(strchr(mnemonic,'A'))
			d1++;
	}
	dest[0]=d1+'0';
	dest[1]=d2+'0';
	dest[2]=d3+'0';
	return dest;
}
/*Returns the binary code of the comp mnemonic.*/
char* code_comp(char* mnemonic){
	int c1=0,c2=0,c3=0,c4=0,c5=0,c6=0;
	int a=0;
	if(mnemonic!=NULL){
		if(!strcmp(mnemonic,"0")){
			c1=c3=c5=1;
		}else if(!strcmp(mnemonic,"1")){
			c1=c2=c3=c4=c5=c6=1;
		}else if(!strcmp(mnemonic,"-1")){
			c1=c2=c3=c5=1;
		}else if(!strcmp(mnemonic,"D")){
			c3=c4=1;
		}else if(!strcmp(mnemonic,"A")||!strcmp(mnemonic,"M")){
			c1=c2=1;
		}else if(!strcmp(mnemonic,"!D")){
			c3=c4=c6=1;
		}else if(!strcmp(mnemonic,"!A")||!strcmp(mnemonic,"!M")){
			c1=c2=c6=1;
		}else if(!strcmp(mnemonic,"-D")){
			c3=c4=c5=c6=1;
		}else if(!strcmp(mnemonic,"-A")||!strcmp(mnemonic,"-M")){
			c1=c2=c5=c6=1;
		}else if(!strcmp(mnemonic,"D+1")){
			c2=c3=c4=c5=c6=1;
		}else if(!strcmp(mnemonic,"A+1")||!strcmp(mnemonic,"M+1")){
			c1=c2=c4=c5=c6=1;
		}else if(!strcmp(mnemonic,"D-1")){
			c3=c4=c5=1;
		}else if(!strcmp(mnemonic,"A-1")||!strcmp(mnemonic,"M-1")){
			c1=c2=c5=1;
		}else if(!strcmp(mnemonic,"D+A")||!strcmp(mnemonic,"D+M")){
			c5++;
		}else if(!strcmp(mnemonic,"D-A")||!strcmp(mnemonic,"D-M")){
			c2=c5=c6=1;
		}else if(!strcmp(mnemonic,"A-D")||!strcmp(mnemonic,"M-D")){
			c4=c5=c6=1;
		}else if(!strcmp(mnemonic,"D&A")||!strcmp(mnemonic,"D&M")){
			//do nothing
		}else if(!strcmp(mnemonic,"D|A")||!strcmp(mnemonic,"D|M")){
			c2=c4=c6=1;
		}
	}

	if(strchr(mnemonic,'M'))
		a++;

	comp[0]=a+'0';
	comp[1]=c1+'0';
	comp[2]=c2+'0';
	comp[3]=c3+'0';
	comp[4]=c4+'0';
	comp[5]=c5+'0';
	comp[6]=c6+'0';

	return comp;
}
/*Returns the binary code of the jump mnemonic.*/
char* code_jump(char* mnemonic){
	int j1=0,j2=0,j3=0;
	if(mnemonic!=NULL){
		if(!strcmp(mnemonic,"JGT")){
			j3++;
		}else if(!strcmp(mnemonic,"JEQ")){
			j2++;
		}else if(!strcmp(mnemonic,"JGE")){
			j2=j3=1;
		}else if(!strcmp(mnemonic,"JLT")){
			j1++;
		}else if(!strcmp(mnemonic,"JNE")){
			j1=j3=1;
		}else if(!strcmp(mnemonic,"JLE")){
			j1=j2=1;
		}else if(!strcmp(mnemonic,"JMP")){
			j1=j2=j3=1;
		}
	}
	jump[0]=j1+'0';
	jump[1]=j2+'0';
	jump[2]=j3+'0';

	return jump;
}