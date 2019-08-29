#ifndef _USING_BF_H
#define _USING_BF_H
void set_flags(const int readable2, const int keep2, 
		const int optimize2, const char* mem_size2,
		const int record2, const int ignore2, const int verbose2);

int translate(FILE *fp);

const char* get_prompt();

const char* get_intern();

const char* get_bf();

void dump_mem(const char* mem, size_t size, size_t index, char type);

int is_bf(const char inpt);

int record_inpt(const char *inpt);

void interp(const char* inpt, char* memory, size_t *mem_size, size_t *d_ptr);

#endif //_USING_BF_H

