#include <stdio.h>		//printf, puts, fprintf
#include <stdlib.h>		//calloc
#include <string.h>		//
#include "../lib/ansi_esc.h"	//colors
#include "bf.h"
#include "bf_vars.h"

//static consts
static const char BF[] = {INC_D, DEC_D, INC_B, DEC_B, OUT_B, IN_B, START_L, END_L, '\0'};
static const char INTERNAL[] = {QUIT, DUMP_C, DUMP_I, CLR_SCR, CLR_MEM, '\0'};

//flags
static int readable = 0;	//Make output readable
static int keep = 1;		//Keep the output
static int optimize = 0; 	//RESERVED
static char mem_size[11] = DEFAULT_MEM_SIZE;
static int record = 0; //record everything the user types
static int ignore = 1; //if 1, ignore interal commands like 'q' to quit or 'd' to dump mem
static int verbose = 0; //verbose output

void set_flags(const int readable2, const int keep2, 
		const int optimize2, const char* mem_size2,
		const int record2, const int ignore2, const int verbose2){
	readable = readable2;
	keep = keep2;
	optimize = optimize2;
	strcpy(mem_size, mem_size2);
	record = record2;
	ignore = ignore2;
	verbose = verbose2;
}

const char* get_prompt(){
	return PROMPT;
}

const char* get_intern(){
	return INTERNAL;
}

const char* get_bf(){
	return BF;
}

void dump_mem(const char* mem, size_t size, size_t index, char type){
	size_t i;
	printf("-- Dump: [Mem Ptr: %s%ld%s] ", BLUE, index, RESET);
	for(i=0; i <= size; i++){
		if(mem[i] != 0){
			if(type == DUMP_C){
				printf("%s%ld%s[%c] ", BLUE, i, RESET, mem[i]);
			} else if(type == DUMP_I) {
				printf("%s%ld%s[%d] ", BLUE, i, RESET, mem[i]);
			}
		}
	}
	puts(" ");
}

int is_bf(const char inpt){
	size_t i;
	for(i=0; i < strlen(BF); i++)
		if(BF[i] == inpt) return 1;
	return 0;
}

int record_inpt(const char *inpt){
	size_t i;
	FILE *fp = NULL;
	fp = fopen("a.bf", "a");
	if(fp == NULL) return 1;
	for(i=0; i < strlen(inpt); i++){
		if(is_bf(inpt[i]))
			fputc(inpt[i], fp);
	}
	fputc('\n', fp);
	fclose(fp);
	return 0;
}

void interp(const char* inpt, char* memory, size_t *mem_size, size_t *d_ptr){
	size_t i, l, deep;
	if(record) record_inpt(inpt);
	for(i=0; i < strlen(inpt); i++){
		if(*d_ptr == *mem_size){
			//expands memory by 10 once the data pointer reaches the end
			memory = realloc(memory, sizeof(char) * 10);
			if(memory != NULL){
				l=*d_ptr;
				*mem_size += 10;
				for(*d_ptr+=10; *d_ptr > l; --*d_ptr) memory[*d_ptr] = 0;
				if(verbose) printf("bfi: Mem expanded to %ld.", *mem_size);
			} else {
				fprintf(stderr, "bfi: Unable to allocate more memory.");
				--*d_ptr;
			}
		}
		switch(inpt[i]){
			case INC_D:
				++*d_ptr;
				break;
			case DEC_D:
				--*d_ptr;
				break;
			case INC_B:
				memory[*d_ptr]++;
				break;
			case DEC_B:
				memory[*d_ptr]--;
				break;
			case OUT_B:
				putchar(memory[*d_ptr]);
				fflush(stdout);
				break;
			case IN_B:
				memory[*d_ptr]=getchar();
				break;
			case START_L:
				if(!memory[*d_ptr]){
					deep = 1;
					while(deep){
						i++;
						if(inpt[i] == START_L) deep++;
						else if(inpt[i] == END_L) deep--;
					}
				}
				break;
			case END_L:
				if(memory[*d_ptr]){
					deep = 1;
					while(deep){
						i--;
						if(inpt[i] == END_L) deep++;
						else if(inpt[i] == START_L) deep--;
					}
				}
				break;
			}
		if(!ignore){
			//Interp specific
			switch(inpt[i]){
				case QUIT:
					free(memory);
					exit(0);
					break;
				case DUMP_C:
					dump_mem(memory, *mem_size, *d_ptr, DUMP_C);
					break;
				case DUMP_I:
					dump_mem(memory, *mem_size, *d_ptr, DUMP_I);
					break;
				case CLR_SCR:
					//Do nothing, UI should handle this
					break;
				case CLR_MEM:
					puts("-- Cleared Memory");
					memset(memory, 0, *mem_size);
					*d_ptr = 0;
					break; 
			}
		}
	}
}


int translate(FILE *fp){
	char inpt = EOF;
	char *mem = calloc(MAX_MEM, sizeof(char));
	if(mem == NULL) return 1; //TODO be better
	strcpy(mem, BOILER_START);
	strcat(mem, mem_size);
	strcat(mem, BOILER_MID);
	do{
		inpt = fgetc(fp);
		switch(inpt){
			case INC_D:
				strcat(mem, "++d_ptr;");
				break;
			case DEC_D:
				strcat(mem, "--d_ptr;");
				break;
			case INC_B:
				strcat(mem, "++*d_ptr;");
				break;
			case DEC_B:
				strcat(mem, "--*d_ptr;");
				break;
			case OUT_B:
				strcat(mem, "putchar(*d_ptr);");
				break;
			case IN_B:
				strcat(mem, "*d_ptr=getchar();");
				break;
			case START_L:
				strcat(mem, "while(*d_ptr){");
				break;
			case END_L:
				strcat(mem, "}");
				break;	
			case EOF:
				strcat(mem, BOILER_END); //TODO check bounds
				break;
		}
		if(readable) strcat(mem, "\n"); //TODO add indents for while and fix for comments
	} while(inpt != EOF);
		//write to disk
		//FILE *fp = NULL;
		fp = NULL;
		fp = fopen("a.c", "w+");
		if(fp == NULL) return 2; //TODO be better
		fprintf(fp, "%s", mem);
		fclose(fp);
		return system("gcc -Wall a.c");
}

