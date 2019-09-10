/* Copyright (c) Adam McKenney 2019, see LICENSE */
#ifndef _USING_BF_VARS_H
#define _USING_BF_VARS_H

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

//Vars
#define MAX_MEM 1000000
#define MEM MAX_MEM/4

#define BOILER_START "/* Auto Generated Code */\n#include <stdio.h>\nint main(){\nint mem["
#define DEFAULT_MEM_SIZE "10000"
#define BOILER_MID "] = {0};\nint *d_ptr=mem;\n"
#define BOILER_END "return 0;\n}"

#define COMPILE_CMD "cc -Wall a.c"

#endif //_USING_BF_VARS_H

