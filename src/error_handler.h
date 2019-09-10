/* Copyright (c) Adam McKenney 2019, see LICENSE */
#ifndef _USING_ERROR_HANDLER_H
#define _USING_ERROR_HANDLER_H

void throw_error(const char* etype, const char* msg, const int exit_code);
void extern_error(const char* etype, const char* msg, const int exit_code);

#endif //_USING_ERROR_HANDLER_H
