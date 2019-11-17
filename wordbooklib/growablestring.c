#define _CRT_SECURE_NO_WARNINGS
#include "growablestring.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// growable string a simplifyed version of dyn-string 
// https://github.com/gcc-mirror/gcc/blob/master/libiberty/dyn-string.c
growable_string_t growable_string_new(int space)
{
    growable_string_t result;
    result = (growable_string_t)malloc(sizeof(struct growable_string));
    if (result == NULL)
    {
        return NULL;
    }
    if (!growable_string_init(result, space))
    {
        free(result);
        return NULL;
    }
    return result;
}

// growable string a simplifyed version of dyn-string 
// https://github.com/gcc-mirror/gcc/blob/master/libiberty/dyn-string.c
int growable_string_append_cstr(growable_string_t dest, const char* s)
{
    int len = strlen(s);

    /* The new length is the old length plus the size of our string, plus
       one for the null at the end.  */
    if (growable_string_resize(dest, dest->length + len) == NULL)
    {
        return 0;
    }
    strcpy(dest->s + dest->length, s);
    dest->length += len;
    return 1;
}

// growable string a simplifyed version of dyn-string 
// https://github.com/gcc-mirror/gcc/blob/master/libiberty/dyn-string.c
int growable_string_append_char(growable_string_t dest, char c)
{
    /* Make room for the extra character.  */
    if (growable_string_resize(dest, dest->length + 1) == NULL)
    {
        return 0;
    }
    /* Append the character; it will overwrite the old NUL.  */
    dest->s[dest->length] = c;
    //  Add a \0 at the end.
    dest->s[dest->length + 1] = '\0';
    // increment
    dest->length += 1;
    return 1;
}

int growable_string_append_int(growable_string_t dest, int i)
{
    /* convert integer to string.  */
    int len = snprintf(NULL, 0, "%i", i);
    char *stri = malloc(len + 1);
    // append
    if (stri)
    {
        snprintf(stri, len + 1, "%i", i);
        int result = growable_string_append_cstr(dest, stri);
        free(stri);
        return result;
    }
    return 0;
}

// growable string a simplifyed version of dyn-string 
// https://github.com/gcc-mirror/gcc/blob/master/libiberty/dyn-string.c
int growable_string_init(struct growable_string *gs_struct_ptr, int space)
{
    /* We need at least one byte in which to store the terminating NUL.  */
    if (space == 0)
    {
        space = 1;
    }
    gs_struct_ptr->s = (char *)malloc(space);
    if (gs_struct_ptr->s == NULL)
    {
        return 0;
    }
    gs_struct_ptr->allocated = space;
    gs_struct_ptr->length = 0;
    gs_struct_ptr->s[0] = '\0';
    return 1;
}

// growable string a simplifyed version of dyn-string 
// https://github.com/gcc-mirror/gcc/blob/master/libiberty/dyn-string.c
growable_string_t growable_string_resize(growable_string_t gs, int space)
{
    int new_allocated = gs->allocated;
    /* Increase SPACE to hold the NUL termination.  */
    ++space;
    /* Increase allocation by factors of two.  */
    while (space > new_allocated)
    {
        new_allocated *= 2;
    }

    if (new_allocated != gs->allocated)
    {
        gs->allocated = new_allocated;
        /* We actually need more space.  */
        char *tmp = (char *)realloc(gs->s, gs->allocated);
        if (tmp == NULL)
        {
            free(gs);
            return NULL;
        }
        gs->s = tmp;
    }
    return gs;
}

void growable_string_delete(growable_string_t gs)
{
    if (gs != NULL)
    {
        if (gs->s != NULL)
        {
            free(gs->s);
        }
        free(gs);
    }
}