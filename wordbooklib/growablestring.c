#include "growablestring.h"
#include <stdlib.h>

// growable string a simplifyed version of dyn-string 
// https://github.com/gcc-mirror/gcc/blob/master/libiberty/dyn-string.c
growable_string_t growable_string_new(int space)
{
    growable_string_t result;
    result = (growable_string_t)malloc(sizeof(struct growable_string));
    if (result == NULL)
        return NULL;
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
        return 0;
    strcpy(dest->s + dest->length, s);
    dest->length += len;
    return 1;
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
        gs->s = (char *)realloc(gs->s, gs->allocated);
        if (gs->s == NULL)
        {
            free(gs);
            return NULL;
        }
    }
    return gs;
}

void growable_string_delete(growable_string_t gs)
{
    free(gs->s);
    free(gs);
}