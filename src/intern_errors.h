/* Copyright (c) Adam McKenney 2019, see LICENSE */
#ifndef _USING_INTERNERNAL_ERRORS_H
#define _USING_INTERNERNAL_ERRORS_H

/* Error macros */
#define E_EXTERNAL "External Error: "
#define E_INTERNAL "Internal Error: "
#define E_USER "User Error: "

/* External */
#define E_ALLOC "Allocating more memory."
#define EXIT_ALLOC 2

#define E_OPENF "Opening file."
#define EXIT_OPENF 3

/* User */
#define E_SYNTAX "Invalid syntax."
#define EXIT_SYNTAX 1

#define E_MISSING_OBRACE "Missing opening brace."
#define E_MISSING_CBRACE "Missing closing brace."
#define E_LBOUNDS "Memory out of bounds. Too low (left)."
#define E_RBOUNDS "Memory out of bounds. Too high (right)."

#endif // _USING_INTERNERNAL_ERRORS_H
