#ifndef __RPRINTF_H__
#define __RPRINTF_H__

//#include <ctype.h>
//#include <string.h>
#include <stdarg.h>

typedef unsigned int size_t;

#define NULL (void*)0

int isdig(int c); //hand-implemented alternative to isdigit()

typedef char* charptr;
typedef int (*func_ptr)(int c);

//common prototype functions

void esp_sprintf(char *buf, char *ctrl, ...);
void esp_vprintf(const func_ptr f_ptr, charptr ctrl, va_list argp);
void esp_printf(const func_ptr f_ptr, charptr ctrl, ...);
void printk(charptr ctrl, ...);
#endif
