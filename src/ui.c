#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/linenoise.h"	//line editing
#include "../lib/ansi_esc.h"	//colors
#include "bf.h"

//Vars
#define MAX_MEM 1000000
#define MEM 10000
#define MEM_S "10000"
#define MAX_FILE_SIZE 1000000
#define FILE_SIZE 100000

//static flags
static int compile = 0;
static size_t memsize = MEM*sizeof(int);
static int ignore = 1; //if 1, ignore interal commands like 'c' to clear or 'd' to dump mem
static int verbose = 0;

int from_file(char *file_name){
	char *inpt = NULL;
	size_t mem_size = MEM*sizeof(int);
	int *memory = calloc(MEM, sizeof(int));
	size_t d_ptr = 0, size, i=0;
	char data = EOF;
	FILE *fp = NULL;
	fp = fopen(file_name, "r");
	inpt = malloc(FILE_SIZE*sizeof(char));
	if(fp == NULL || inpt == NULL || memory == NULL){
		fprintf(stderr, "bfi: %s:", file_name);
		perror("");
		return 1;
	} else if(compile){
		free(memory);
		free(inpt);
		translate(fp);
		fclose(fp);
		return 0;
	}
	size = FILE_SIZE  * sizeof(char);
	do{
		data = fgetc(fp);
		inpt[i] = data;
		i++;
		if(i >= size/sizeof(char)-1){
			if(size + (sizeof(char)*FILE_SIZE) > MAX_FILE_SIZE * sizeof(char)){
				fprintf(stderr, "bfi: %s: File size over hard limit of %ld bytes. Truncating.\n",
					file_name, MAX_FILE_SIZE * sizeof(char));
				i--;
				data = EOF;
				inpt[i] = data;
			} else {
				if(verbose) putchar('!'); //TODO FIXME
				fflush(stdout);
				inpt = realloc(inpt, size + (sizeof(char) * FILE_SIZE));
				if(inpt == NULL){ perror("bfi "); return -1;}
				size += sizeof(char) * FILE_SIZE;
			}
		}
	} while(data != EOF);
	inpt[--i] = '\0';
	fclose(fp);
	interp(inpt, memory, &mem_size, &d_ptr);

	free(memory);
	free(inpt);
	return 0;	
}
int cli(){
	char *inpt = NULL;
	size_t mem_size = memsize;
	int *memory = malloc(mem_size);
	if(memory == NULL){ perror("bfi: "); return -1;}
	memset(memory, 0, mem_size);
	size_t d_ptr = 0;

	printf("-- Brainfuck Interpreter running with %ld bytes of dynamic memory --\n", mem_size);
	printf("-- I only recognize '%s' ", get_bf());
	if(!ignore) printf("and the internal commands '%s' --\n", get_intern());
	else printf("--\n");
	printf("-- type 'q' to quit --\n");
	
	if(memory == NULL) return 1; //TODO be better
	while(1){
		inpt = linenoise(get_prompt());
		if(inpt != NULL){
			if(!strcmp(inpt, "q") || !strcmp(inpt, "quit") || !strcmp(inpt, "e") || !strcmp(inpt, "exit")) exit(0);
			else if(!ignore && (!strcmp(inpt, "c") || !strcmp(inpt, "clear") || !strcmp(inpt, "cls"))) linenoiseClearScreen();
			linenoiseHistoryAdd(inpt);
			interp(inpt, memory, &mem_size, &d_ptr);
		}
	}
	return 0;
}

int main(int argc, char *argv[]){
	//flags
	int record = 0; //record everything the user types
	int newline = 0;
	int keep = 0;
	int optimize = 0;
	
	//handle args
	int i;
	char mem_size[11] = MEM_S;
	for(i=1; i < argc; i++){
		if(!strcmp(argv[i], "-r") || !strcmp(argv[i], "--record")) record = 1;
		else if(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--debug")) ignore = 0;
		else if(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")) verbose = 1;
		else if(!strcmp(argv[i], "-c") || !strcmp(argv[i], "--compile")){
			compile = 1;
			set_flags(newline, keep, optimize, mem_size, record, ignore, verbose);
			from_file(argv[i+1]);
		}
		else if(!strcmp(argv[i], "-n") || !strcmp(argv[i], "--newline")) newline = 1;
		else if(!strcmp(argv[i], "-k") || !strcmp(argv[i], "--keep")) keep = 1;
		else if(!strcmp(argv[i], "-o") || !strcmp(argv[i], "--optimize")) optimize = 1;
		else if(!strcmp(argv[i], "-m") || !strcmp(argv[i], "--memory")){
			strncpy(mem_size, argv[++i], 11);
			mem_size[10] = '\0';
			if(atol(mem_size) != 0){ memsize = atol(mem_size); memsize = memsize*sizeof(char); }
		}
		else if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) { puts("bfi -[rdvh] <optional_file>\n record to file, turn on debugging features, verbose output, and help respectivley."); return 0;}
		else if(argv[i][0] == '-'){
			fprintf(stderr, "bfi: Unknown arg '%s'\n", argv[i]);
			return 1;
		} else {
			set_flags(newline, keep, optimize, mem_size, record, ignore, verbose);
			return from_file(argv[i]);
		}
	}
	set_flags(newline, keep, optimize, mem_size, record, ignore, verbose);
	return cli();
}


