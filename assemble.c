/* fix make file befor giving project remove o and x folders*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dict.h"
#include "symbtab.h"
#include "passes.h"

/* General Constants and Definitions */
#define MAX_MACHINE_ORDERS 580  /* Based on RAM size 4096 and Each Machine Order is 60 bits aka 6 RAM units */
#define MAX_DATA_ORDERS 100     /* its 680 from which 580 to machine and 100 for data */
#define MAX_FILENAME_LENGTH 100
#define MAX_STATEMENT_LENGTH 82


/* Code Image / Data Image / Symbol Table / ICF & DCF */
MachineOrder *CODE_IMAGE[MAX_MACHINE_ORDERS];
int DATA_IMG[MAX_DATA_ORDERS];
Tlist SYMB_TABLE = { NULL };
static int ICF,DCF;

/* Function Declarations */
int readfile(char *);
int writefiles(char *);
void printTable(Tlinkptr);
void freeTable(Tlinkptr);

/* Assembler main runfile */
int main(int argc, char *argv[]) {
	while (*++argv){
		if(readfile(*argv)){
			printTable(SYMB_TABLE.head);
			writefiles(*argv);
		} 
	}
	freeTable(SYMB_TABLE.head);
	return 0;
}


/* Declaring Global Variables and Functions to be used */
int readfile(char *file){
	FILE *fd;
	char file_ext[MAX_FILENAME_LENGTH];
	
	char statement[MAX_STATEMENT_LENGTH];
	int statement_cnt=0;

	int IC=100,DC=0;

	strcpy(file_ext,"x/");			/*	remove the x directory*/
	strcat(file_ext,file);
	strcat(file_ext,".as");
	if(!(fd=fopen(file_ext,"r+"))){ /* writable problem accessing file*/
		printf("Cannot find or open the file %s .\n",file_ext);
		return NOCHANGE;
	}

	printf("The file %s was found, intiating first pass:\n",file_ext);
	while(fgets(statement,MAX_STATEMENT_LENGTH,fd)){
		statement_cnt++;
		if(firstpass(statement,statement_cnt,&IC,&DC,&(SYMB_TABLE.head))==3){
			printf("Errors were found in first pass of file %s, terminating proccess for this file.",file_ext);
			fclose(fd);
			return NOCHANGE;
		}	
	}
	ICF=IC;
	DCF=DC;
	update_data_symbols(ICF,&(SYMB_TABLE.head));	/*	Update in Symbol Table every data Symbol using +ICF */
	statement_cnt=0;

	rewind(fd);
	printf("First pass of %s finished successuly, intiating second pass:\n",file_ext);
	while(fgets(statement,MAX_STATEMENT_LENGTH,fd)){
		secondpass(statement,&IC,&DC);
	}
	/*if(errors){
		printf("The following Errors were found in %s:\n",fileext);
		printf("Terminating Proccess for this file.");
		return 0;
	}*/
	fclose(fd);
	return 1;
}

int writefiles(char *file){printf("write files\n"); return 1;}
void printTable(Tlinkptr r){
	if(r){
		printf("Symbol: %s Value: %d Code: %d Data: %d entry: %d extern: %d Next: %s \n",(*r).symbol,(*r).value,(*r).is_code,(*r).is_data,(*r).is_entry,(*r).is_extern,(*((*r).next)).symbol);
		printTable((*r).next);
	}
}

void freeTable(Tlinkptr r){
	if(r){
		printf("There is %s\n",(*((*r).next)).symbol);
		freeTable((*r).next);
		free(r);
	}
}
