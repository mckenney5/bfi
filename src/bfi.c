#include <stdio.h>		//printf, puts, fprintf
#include <stdlib.h>		//calloc
#include <string.h>		//
#include "../lib/linenoise.h"	//line editing
#include "../lib/ansi_esc.h"	//colors

//Vars
#define MAX_MEM 1000000
#define MEM 10000
#define MAX_FILE_SIZE 1000000
#define FILE_SIZE 1000

//BF
#define INC_D '>'
#define DEC_D '<'
#define INC_B '+'
#define DEC_B '-'
#define OUT_B '.'
#define IN_B ','
#define START_L '['
#define END_L ']'

//interp specific
#define PROMPT "~ "
#define QUIT 'q'
#define DUMP_C 'd'
#define DUMP_I 'D'
#define CLR_SCR 'c'
#define CLR_MEM 'C'
//End

//static consts
static const char BF[] = {INC_D, DEC_D, INC_B, DEC_B, OUT_B, IN_B, START_L, END_L, '\0'};
static const char INTERNAL[] = {QUIT, DUMP_C, DUMP_I, CLR_SCR, CLR_MEM, '\0'};

//flags
static int record = 0; //record everything the user types
static int ignore = 1; //if 1, ignore interal commands like 'q' to quit or 'd' to dump mem
static int verbose = 0; //verbose output

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
					linenoiseClearScreen();
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

int cli(char *file_name){
	char *inpt = NULL;
	size_t mem_size = MEM*sizeof(char);
	char *memory = calloc(MEM, sizeof(char));
	size_t d_ptr = 0, size, i=0;
	char data = EOF;

	if(file_name){
		FILE *fp = NULL;
		fp = fopen(file_name, "r");
		inpt = malloc(FILE_SIZE*sizeof(char));
		if(fp == NULL || inpt == NULL){
			fprintf(stderr, "bfi: %s:", file_name);
			perror("");
			return 1;
		}
		size = FILE_SIZE  * sizeof(char);
		do{
			data = fgetc(fp);
			inpt[i] = data;
			i++;
			if(i >= size){
				if(size + (sizeof(char)*1000) > MAX_FILE_SIZE * sizeof(char)){
					fprintf(stderr, "bfi: %s: File size over hard limit of %ld bytes. Truncating.\n", file_name, MAX_FILE_SIZE * sizeof(char));
					i--;
					data = EOF;
					inpt[i] = data;
				} else {
					inpt = realloc(inpt, sizeof(char) * 1000);
					size += sizeof(char) * 1000;
				}
			}
		} while(data != EOF);
		inpt[--i] = '\0';
		interp(inpt, memory, &mem_size, &d_ptr);
		return 0;	
	}

	printf("-- Brainfuck Interpreter running with %ld bytes of dynamic memory --\n", mem_size);
	printf("-- I only recognize '%s' and the internal commands '%s' --\n", BF, INTERNAL);
	
	if(memory == NULL) return 1; //TODO be better
	while(1){
		inpt = linenoise(PROMPT);
		linenoiseHistoryAdd(inpt);
		if(inpt != NULL) interp(inpt, memory, &mem_size, &d_ptr);
	}
	return 0;
}

int main(int argc, char *argv[]){
	//handle args
	int i;
	for(i=1; i < argc; i++){
		if(!strcmp(argv[i], "-r") || !strcmp(argv[i], "--record")) record = 1;
		else if(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--debug")) ignore = 0;
		else if(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")) verbose = 1;
		else if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) { puts("bfi -[rdvh] <optional_file>\n record to file, turn on debugging features, verbose output, and help respectivley."); return 0;}
		else if(argv[i][0] == '-'){
			fprintf(stderr, "bfi: Unknown arg '%s'\n", argv[i]);
			return 1;
		} else return cli(argv[i]);
			
	}
	return cli(NULL);
}

