/* Copyright (c) Adam McKenney 2019, see LICENSE */
#include <stdio.h>
#include <stdlib.h>

void throw_error(const char* etype, const char* msg, const int exit_code){
	if(!etype || !msg){
		fprintf(stderr, "Internal Error: In function throw_error, string is (null).\n");
		exit(-1);
	} else
		fprintf(stderr, "%s%s\n", etype, msg);
	if(exit_code) exit(exit_code);
}

void extern_error(const char* etype, const char* msg, const int exit_code){
	throw_error(etype, msg, 0);
	perror("");
	if(exit_code) exit(exit_code);
}

