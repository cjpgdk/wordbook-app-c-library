#ifndef GROWABLESTRING_H
#define GROWABLESTRING_H
#include <stdlib.h>


// growable string a simplifyed version of dyn-string 
// https://github.com/gcc-mirror/gcc/blob/master/include/dyn-string.h
typedef struct growable_string
{
    size_t allocated;	/* The amount of space allocated for the string.  */
    size_t length;		/* The actual length of the string.  */
    char *s;		/* The string itself, NUL-terminated.  */
} *growable_string_t;

// growable string a simplifyed version of dyn-string 
// https://github.com/gcc-mirror/gcc/blob/master/libiberty/dyn-string.c
growable_string_t growable_string_new(size_t space);
// growable string a simplifyed version of dyn-string 
// https://github.com/gcc-mirror/gcc/blob/master/libiberty/dyn-string.c
int growable_string_append_cstr(growable_string_t dest, const char *s);
// growable string a simplifyed version of dyn-string 
// https://github.com/gcc-mirror/gcc/blob/master/libiberty/dyn-string.c
int growable_string_append_char(growable_string_t dest, char c);
// growable string a simplifyed version of dyn-string 
// https://github.com/gcc-mirror/gcc/blob/master/libiberty/dyn-string.c
int growable_string_init(struct growable_string *gs_struct_ptr, size_t space);
// growable string a simplifyed version of dyn-string 
// https://github.com/gcc-mirror/gcc/blob/master/libiberty/dyn-string.c
growable_string_t growable_string_resize(growable_string_t gs, size_t space);
void growable_string_delete(growable_string_t gs);
int growable_string_append_int(growable_string_t dest, int i);
#endif // !GROWABLESTRING_H