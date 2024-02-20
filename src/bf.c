/* Copyright (c) Adam McKenney 2024, see LICENSE */
#include <stdio.h>		//printf, puts, fprintf
#include <stdlib.h>		//calloc
#include <string.h>		
#include <unistd.h>
#include <termios.h>
#include "../lib/ansi_esc.h"	//colors
#include "bf.h"
#include "bf_vars.h"
#include "intern_errors.h"
#include "error_handler.h"

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

static struct termios orig_termios;

static void disable_rm(){
/* disables raw mode for the terminal */
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
static void enable_rm(){
/* Allows us to get input one byte at a time */
	tcgetattr(STDIN_FILENO, &orig_termios);
	struct termios raw = orig_termios;
	raw.c_lflag &= ~(ICANON); // | ECHO); //should echo be off?
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

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

void dump_mem(const int* mem, size_t size, size_t index, char type){
	size_t i;
	printf("-- Dump: [Mem Ptr: %s%ld%s] ", BLUE, index, RESET);
	for(i=0; i <= size/sizeof(int); i++){
		if(mem[i] != 0){
			if(type == DUMP_C){
				char temp[3] = {'\0'};
				temp[0] = '\\';
				switch(mem[i]){
					case '\n':
						temp[1] = 'n';
						break;
					case '\t':
						temp[1] = 't';
						break;
					case '\0':
						temp[1] = '0';
						break;
					case '\r':
						temp[1] = 'r';
						break;
					case '\b':
						temp[1] = 'b';
						break;
					default:
						temp[0] = mem[i];
				}
				temp[3] = '\0';
				printf("%s%ld%s[%s] ", BLUE, i, RESET, temp);
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
	if(fp == NULL) extern_error(E_EXTERNAL, E_OPENF, EXIT_OPENF);
	for(i=0; i < strlen(inpt); i++){
		if(is_bf(inpt[i]))
			fputc(inpt[i], fp);
	}
	fputc('\n', fp);
	fclose(fp);
	return 0;
}

void interp(const char* inpt, int* memory, size_t *mem_size, size_t *d_ptr){
	long i;
	size_t l, deep;
	char c = 0;
	if(record) record_inpt(inpt);
	for(i=0; i < (long)strlen(inpt); i++){
		if(*d_ptr == *mem_size){
			//expands memory by 10 once the data pointer reaches the end
			memory = realloc(memory, (*mem_size +10) * sizeof(int));
			if(memory != NULL){
				l=*d_ptr;
				*mem_size += 10*sizeof(int);
				for(*d_ptr+=10; *d_ptr > l; --*d_ptr) memory[*d_ptr] = 0;
				if(verbose) printf("bfi: Mem expanded to %ld.", *mem_size);
			} else {
				extern_error(E_EXTERNAL, E_ALLOC, 0);
				throw_error(E_USER, E_RBOUNDS, 0);
				--*d_ptr;
			}
		}
		switch(inpt[i]){
			case INC_D:
				++*d_ptr;
				break;
			case DEC_D:
				if(*d_ptr != 0) --*d_ptr;
				else throw_error(E_USER, E_LBOUNDS, 0);
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
				enable_rm();
				read(STDIN_FILENO, &c, 1);
				disable_rm();
				memory[*d_ptr] = c;
				putchar('\n');
				break;
			case START_L:
				if(!memory[*d_ptr]){
					deep = 1;
					while(deep){
						i++;
						if(i > (long)strlen(inpt)){
							fprintf(stderr, "bfi: Error missing '%c'\n", END_L);
							return;	
						}
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
						if(i < 0){
							fprintf(stderr, "bfi: Error missing '%c'\n", START_L);
							return;
						}
	
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
	if(mem == NULL) extern_error(E_EXTERNAL, E_ALLOC, EXIT_ALLOC);
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
		fp = NULL;
		fp = fopen("a.c", "w+");
		if(fp == NULL) extern_error(E_EXTERNAL, E_OPENF, EXIT_OPENF);
		fprintf(fp, "%s", mem);
		fclose(fp);
		free(mem);
		return system(COMPILE_CMD);
}

