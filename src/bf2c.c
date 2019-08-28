#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "../lib/linenoise.h"

//Vars
#define MAX_MEM 1000000
#define MEM MAX_MEM/4

#define INC_D '>'
#define DEC_D '<'

#define INC_B '+'
#define DEC_B '-'
#define OUT_B '.'
#define IN_B ','

#define START_L '['
#define END_L ']'

#define BOILER_START "/* Auto Generated Code */\n#include <stdio.h>\nint main(){\nchar mem["
#define DEFAULT_MEM_SIZE "10000"
#define BOILER_MID "] = {0};\nchar *d_ptr=mem;\n"
#define BOILER_END "return 0;\n}"

static int readable = 0;	//Make output readable
static int keep = 1;		//Keep the output
static int optimize = 0; 	//RESERVED
static char mem_size[11] = DEFAULT_MEM_SIZE;

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

void help(){
	puts("bf2c [OPTIONS] [STDIN]");
	puts("Options -rkohm");
	puts("We take brain fuck code from stdin and convert it to C");
	puts("try: 'cat example.bf | ./bf2c -r");
}

int main(int argc, char *argv[]){
	//handle args
	int i, l;
	if(argc == 1){
		return translate(stdin);
	} else {
		for(i=1; i < argc; i++){
			if(argv[i][0] == '-'){
				switch(argv[i][1]){
					case 'r':
						readable = 1;
						break;
					case 'k':
						keep = 0;
						break;
					case 'o':
						optimize++;
						break;
					case 'h':
						help();
						return 0;
						break;
					case 'm':
						for(l=0; l < 11 && l+1 < strlen(argv[i]); l++)
							mem_size[l] = argv[i][l+2];
						if(atoi(mem_size) == 0) strcpy(mem_size, DEFAULT_MEM_SIZE);
						break;
					default:
						fprintf(stderr, "bf2c: Invalid syntax '%s'.\n", argv[i]);
						return -1;
						break;
				}
			} else {
				fprintf(stderr, "bf2c: Invalid syntax '%s'.\n", argv[i]);
				return -1;
			}
		}
	}
	return translate(stdin);
}
