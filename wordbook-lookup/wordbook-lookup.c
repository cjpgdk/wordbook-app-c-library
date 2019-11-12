#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"wordbooklib.h"
#include <json-c/json.h>

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif


int main(int argc, char *argv[])
{
#if _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
   
    // download all dictionaries
    wordbook_array_dictionary *dict_array;
    dict_array = wordbook_get_dictionaries();

    // loop over all dictionaries, and print their info!
    for (size_t i = 0; i < dict_array->count; i++)
    {
        wordbook_dictionary dict = dict_array->dicts[i];
        //dict.info;
        //dict.alphabet;
        //dict.url;
        printf("id .......: %s\n", dict.id);
        printf("long name : %s\n", dict.long_name);
        printf("short name: %s\n\n", dict.short_name);
    }

    // free all memory usen by the downloaded dictionaries
    wordbook_array_dictionary_free(dict_array);
    return 0;
}