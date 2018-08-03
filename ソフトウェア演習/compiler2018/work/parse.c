#include <stdio.h>
#include <stdlib.h>
#include <getsym.h>
#include <string.h>
extern TOKEN tok;
extern FILE *infile;
extern FILE *outfile;
void loadaddrsearch(void);//グローバル変数かローカル変数かでload命令を変える関数
void paramlist(int a);
void inblock(void);
void expression1(void);
void term(void);
void factor(void);
void error(char *s);
void statement(void);
void conditions(char *end);//条件式を管理する変数
void expression(void);//式を管理する関数
void outblock(void);//変数を管理する関数
int addrsearch(void);//変数のアドレスを返り値にもつ関数
void datasection(void);//data命令を管理する関数
int varflag = 0;//0ならaddrsearchを呼び出した時に局所変数から参照して1なら局所変数を飛ばしてglobal変数から参照する
int datanum = 0;//data命令の数を管理する変数
int funcnum = 0;//関数の個数を管理する変数
int addr = 0;//global変数の個数を管理する変数
int relativenum = 0;//局所変数の個数を管理するアドレス
int addrvar = 0;//adrを返り値としてもつ関数の返り値格納用の変数
int whilecounter = 0;//whileがいくつ出てくるかを数えるカウンター
int ifcounter = 0;//ifがいくつ出てくるかを数えるカウンター
int dataimme[32];//data命令を適用した実際の即値
char addrchar[32] = {0};//store命令を使用する時にstore先のアドレスを格納する文字列
typedef struct{//global変数の構造体
	int adr;
	char v[32];
}s_entry;
typedef struct{//関数の名前の管理用
	int funcnums;
	int funcvar;//局所変数の相対アドレスのスタートを管理する変数。
	char func[32];
}s_func;
typedef struct{
	int relativeadr;
	char y[32];
}s_localentry;
s_localentry s_local[32];
s_func s_functable[32];
s_entry s_table[32];

void compiler(void){
	init_getsym();

	getsym();

	if (tok.attr == RWORD && tok.value == PROGRAM){

		getsym();

		if (tok.attr == IDENTIFIER){

			getsym();

			if (tok.attr == SYMBOL && tok.value == SEMICOLON){

				getsym();
				outblock();
				statement();

				getsym();
				fprintf(outfile,"halt\n");
				datasection();
				if (tok.attr == SYMBOL && tok.value == PERIOD){
					fprintf(stderr, "Parsing Completed. No errors found.\n");
				} else error("At the end, a period is required.");
			}else error("After program name, a semicolon is needed.");
		}else error("Program identifier is needed.");
	}else error("At the first, program declaration is required.");
}

void error(char *s){
	fprintf(stderr, "%s\n", s);
	exit(1);
}

//
// Parser
//
void paramlist(int a){//あとでPROCEDUREが二つ以上あった時の処理に対応させる
	getsym();
	a = 0;
	while(1){
		expression();
		fprintf(outfile,"push r0\n");
		a++;
		if(tok.attr == SYMBOL && tok.value == RPAREN){
			break;
		}
		getsym();
	}
	a = relativenum - a;
	fprintf(outfile,"addi sp,%d\n",a);
	getsym();
}
void outblock(void){
	if(tok.attr == RWORD && tok.value == VAR){
		while(1){
			getsym();
			if(tok.attr == SYMBOL && tok.value == COMMA){
				continue;
			}else if(tok.attr == SYMBOL && tok.value == SEMICOLON ){
				break;
			}else{
			strcpy(s_table[addr].v,tok.charvalue);
			s_table[addr].adr = addr;
			addr++;
		}

	}
	fprintf(outfile,"addi br,%d\n",addr);
	getsym();
}
if(tok.attr == RWORD && tok.value == PROCEDURE){
while(tok.attr == RWORD && tok.value == PROCEDURE){
	getsym();
	if(tok.attr == IDENTIFIER){
		strcpy(s_functable[funcnum].func,tok.charvalue);
		funcnum++;
	}else{
		error("syntax error");
	}
	s_functable[funcnum-1].funcvar = relativenum;
	getsym();
	inblock();
	getsym();//セミコロンを取って来るはず
	getsym();//PROCEDUREかどうかを確認するはず

}
}else{
	fprintf(outfile,"addi sp,%d\n",addr+relativenum);
}
fprintf(outfile,"MAIN:\n");
varflag++;
statement();
}
void inblock(void){
	if(tok.attr == SYMBOL && tok.value == LPAREN){
		getsym();
	}else{
		error("syntax error");
	}
	while(1){
		if(tok.attr == IDENTIFIER){//例外処理をあとで加える
			strcpy(s_local[relativenum].y,tok.charvalue);
			s_local[relativenum].relativeadr = relativenum;
			relativenum++;
		}
		getsym();
		if(tok.attr == SYMBOL && tok.value == COMMA){
			getsym();
		}else{
			break;
		}
	}
	if(tok.attr == SYMBOL && tok.value == RPAREN){
		getsym();
	}
	if(tok.attr == SYMBOL && tok.value == SEMICOLON){
		getsym();
	}
	//動的リンクを貼る
	//
	if(tok.attr == RWORD && tok.value == VAR){
		getsym();
	}else{
		//あとで例外処理
	}
	while(1){
		if(tok.attr == IDENTIFIER){
			strcpy(s_local[relativenum].y,tok.charvalue);
			s_local[relativenum].relativeadr = relativenum;
			relativenum++;
		}
		getsym();
		if(tok.attr == SYMBOL && tok.value == COMMA){
			getsym();
		}else{
			break;
		}
	}
	fprintf(outfile,"addi sp,%d\n",addr+relativenum);
	fprintf(outfile,"jmp MAIN\n");
	fprintf(outfile,"F%d:\n",funcnum-1);
	getsym();
	statement();
	fprintf(outfile,"load br,%d(br)\n",relativenum);
	fprintf(outfile,"return\n");
}
void statement(void){

  if(tok.attr == RWORD && tok.value == WHILE){
		whilecounter ++;
		char L[5] = "L";
		char startbuf[32];
		char endbuf[32];
		char start[32];
		char end[32];
		sprintf(startbuf,"%d",2*whilecounter-1);
		sprintf(endbuf,"%d",2*whilecounter);
		sprintf(start,"%s%s",L,startbuf);
		sprintf(end,"%s%s",L,endbuf);
		fprintf(outfile,"%s:\n",start);
		getsym();
		conditions(end);
		if(tok.attr == RWORD && tok.value == DO){
			getsym();
			statement();
		}

		fprintf(outfile,"jmp %s\n",start);
		fprintf(outfile,"%s:\n",end);
		getsym();
	}
	else if(tok.attr == RWORD && tok.value == IF){
		ifcounter ++;
		char G[5] = "G";
		char ifstartbuf[32];
		char ifendbuf[32];
		char ifstart[32];
		char ifend[32];
		sprintf(ifstartbuf,"%d",2*ifcounter-1);
		sprintf(ifendbuf,"%d",2*ifcounter);
		sprintf(ifstart,"%s%s",G,ifstartbuf);
		sprintf(ifend,"%s%s",G,ifendbuf);
		getsym();
		conditions(ifend);
		if(tok.attr == RWORD && tok.value == THEN){
			getsym();
			statement();
			fprintf(outfile,"jmp %s\n",ifstart);

		}
		if(tok.attr == RWORD && tok.value == END){
			getsym();
		}
		fprintf(outfile,"%s:\n",ifend);
		if(tok.attr == RWORD && tok.value == ELSE){

			getsym();
			statement();
		}
		fprintf(outfile,"%s:\n",ifstart);
	}
  else if (tok.attr == RWORD && tok.value == BEGIN) {
    getsym();
    statement();
		while(tok.attr == SYMBOL && tok.value == SEMICOLON){
			getsym();
			statement();
		}
  }
	else if(tok.attr == RWORD && tok.value == VAR){
		while(1){
			getsym();
			if(tok.attr == SYMBOL && tok.value == COMMA){
				continue;
			}else if(tok.attr == SYMBOL && tok.value == SEMICOLON ){
				break;
			}else{
			strcpy(s_table[addr].v,tok.charvalue);
			s_table[addr].adr = addr;
			addr++;
		}
	}
	getsym();
	statement();
}
else if(tok.attr == RWORD && tok.value == WRITE){
	while(1){
		getsym();
		addrvar=addrsearch();
		fprintf(outfile, "load r0,%s\n",addrchar);
		fprintf(outfile,"writed r0\n",addrvar);
		fprintf(outfile,"loadi r3,' '\n");
		fprintf(outfile,"writec r3\n");
		getsym();
		if(tok.value != COMMA){
			break;
		}
	}
	fprintf(outfile,"loadi r3,'\\n'\n");
	fprintf(outfile,"writec r3\n");
	statement();
}
else if(tok.attr == IDENTIFIER){
	addrvar = addrsearch();
	getsym();
	if(tok.attr == SYMBOL && tok.value == BECOMES){
		getsym();
	expression();
	fprintf(outfile,"store r0,%s\n",addrchar);
}else if(tok.attr == SYMBOL && tok.value == LPAREN){
	fprintf(outfile,"loadr r1,sp\n");//paramlistの処理ではconditionsが出てこないので一時的にparamlistの処理終了後のbrを退避させる為に使用
	fprintf(outfile,"addi r1,%d\n",1);
	paramlist(addrvar);
	//fprintf();
	fprintf(outfile,"push br\n");
	fprintf(outfile,"loadr br,r1\n");
	fprintf(outfile,"call F%d\n",addrvar);
	fprintf(outfile,"subi sp,%d\n",relativenum+1);
}
}
if (tok.attr == RWORD && tok.value == END) {
}

}

void conditions(char *end){ //条件を書くための関数。その際に必ず左辺の演算結果をr1,右辺の演算結果をr2としておく。
	expression();
	fprintf(outfile,"loadr r1,r0\n");
	if(tok.attr == SYMBOL && tok.value == LESSTHAN){
		getsym();
		expression();
		fprintf(outfile, "loadr r2,r0\n");
		fprintf(outfile,"cmpr r1,r2\n");
		fprintf(outfile,"jge %s\n",end);
	}
	else if(tok.attr == SYMBOL && tok.value == GRTRTHAN){
		getsym();
		expression();
		fprintf(outfile, "loadr r2,r0\n");
		fprintf(outfile,"cmpr r1,r2\n");
		fprintf(outfile,"jle %s\n",end);
	}
	else if(tok.attr == SYMBOL && tok.value == LESSEQL){
		getsym();
		expression();
		fprintf(outfile, "loadr r2,r0\n");
		fprintf(outfile,"cmpr r1,r2\n");
		fprintf(outfile,"jgt %s\n",end);
	}
	else if(tok.attr == SYMBOL && tok.value == GRTREQL){
		getsym();
		expression();
		fprintf(outfile, "loadr r2,r0\n");
		fprintf(outfile,"cmpr r1,r2\n");
		fprintf(outfile,"jlt %s\n",end);
	}
	else if(tok.attr == SYMBOL && tok.value == EQL){
		getsym();
		expression();
		fprintf(outfile, "loadr r2,r0\n");
		fprintf(outfile,"cmpr r1,r2\n");
		fprintf(outfile,"jnz %s\n",end);
	}
	else{
		error("条件式になっていません");
	}
}




int addrsearch(void){ //変数がどの番地（絶対アドレス番地）に入っているかを探すための関数
	int i = 0;
	if(varflag  == 0){
	for(i=0;i<32;i++){
	if(strcmp(tok.charvalue,s_local[i].y)==0){
		sprintf(addrchar,"%d(br)",i);
		return 0;
	}
}
}
for(i=0;i<32;i++){
	if(strcmp(tok.charvalue,s_table[i].v)==0){
		sprintf(addrchar,"%d",i);
		return 0;
	}
}
for(i = 0;i<32;i++){
	if(strcmp(tok.charvalue,s_functable[i].func)==0){
		return i;
	}
}

/*if(i==32){
	error("変数が宣言されていません");
}*/
return i;
}



void expression(void){//最後の演算結果をr0に入れるための関数
	expression1();
	fprintf(outfile,"pop r0\n");
}
void datasection(void){
	int j = 0;
	for(j = 0;j < datanum; j++){
		fprintf(outfile,"x%d: data %d\n",j,dataimme[j]);
	}
}
void expression1(void){//スタックを使った演算を行うための関数
	term();
	if(tok.attr == SYMBOL && tok.value == PLUS){
		getsym();
		term();
		fprintf(outfile,"pop r0\n");
		fprintf(outfile,"pop r3\n");
		fprintf(outfile,"addr r3,r0\n");
		fprintf(outfile,"push r3\n");
	}
	else if(tok.attr == SYMBOL && tok.value == MINUS){
		getsym();
		term();
		fprintf(outfile,"pop r0\n");
		fprintf(outfile,"pop r3\n");
		fprintf(outfile,"subr r3,r0\n");
		fprintf(outfile,"push r3\n");
	}
}

void term(void){
	factor();
	if(tok.attr == SYMBOL && tok.value == TIMES){
		getsym();
		factor();
		fprintf(outfile,"pop r0\n");
		fprintf(outfile,"pop r3\n");
		fprintf(outfile,"mulr r3,r0\n");
		fprintf(outfile,"push r3\n");

	}
	else if(tok.attr == RWORD && tok.value == DIV){
		getsym();
		factor();
		fprintf(outfile,"pop r0\n");
		fprintf(outfile,"pop r3\n");
		fprintf(outfile,"divr r3,r0\n");
		fprintf(outfile,"push r3\n");
	}
}

void factor(void){
	if(tok.attr == IDENTIFIER){
		loadaddrsearch();
		fprintf(outfile,"push r0\n");
	}else if(tok.attr == NUMBER){
		if(tok.value >= 32768){
			dataimme[datanum] = tok.value;
			fprintf(outfile,"load r0,x%d\n",datanum);
			fprintf(outfile,"push r0\n");
			datanum++;
		}else{
		fprintf(outfile,"loadi r0,%d\n",tok.value);
		fprintf(outfile,"push r0\n");
	}
	}else if(tok.attr == SYMBOL && tok.value == LPAREN){
		getsym();
		expression1();
	}
	getsym();
}



void loadaddrsearch(void){
	int i = 0;
	if(varflag == 0){
	for(i=0;i<32;i++){
	if(strcmp(tok.charvalue,s_local[i].y)==0){
		fprintf(outfile,"load r0,%d(br)\n",i);
		return;
	}
}
}
for(i=0;i<32;i++){
	if(strcmp(tok.charvalue,s_table[i].v)==0){
		fprintf(outfile,"load r0,%d\n",i);
		return;
	}
}
}
