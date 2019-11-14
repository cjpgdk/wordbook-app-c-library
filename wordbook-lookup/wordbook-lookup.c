#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include"wordbooklib.h"
#include <json-c/json.h>
#include "wordbook-lookup.h"
#ifdef _MSC_VER
#include "getopt.h"
#else
#include <getopt.h>
#endif // _MSC_VER


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

    bool print_dicts = false, print_help = false, 
         print_suggests = false;
    char* print_dict = NULL, *print_suggest = NULL;
    char c;
    while ((c = getopt(argc, argv, "D:ds:h")) != -1)
    {
        switch (c)
        {
        case 'd':
            print_dicts = true;
            break;
        case 'D':
            print_dicts = true;
            print_dict = optarg;
            break;
        case 's':
            print_suggests = true;
            print_suggest = optarg;
            break;
        case 'h':
            print_help = true;
            break;
        default:
            break;
        }
    }

    // print the help text
    if (print_help)
    {
        print_help_text(argv[0]);
        return EXIT_SUCCESS;
    }

    // print dictionaries.
    // if print_suggests that we ship and use the dict_id in the suggestions lookup
    if (print_dicts && !print_suggests)
    {
        print_all_dictionaries(print_dict);
        return EXIT_SUCCESS;
    }

    if (print_suggests)
    {
        printf("%s\n", print_suggest);
        printf("%s\n", print_dict);
        printf("%s\n", NULL);
        wordbook_get_suggestions(print_suggest, print_dict);
        return EXIT_SUCCESS;
    }

    // if we get here we are missing some args, show help
    print_help_text(argv[0]);
    return EXIT_SUCCESS;
}



// print help text
void print_help_text(const char *app)
{
    printf("Usage: ./%s\n\n", app);
    printf(" - ./%s -d\n", app);
    printf("\tList all dictionaries\n");
    printf(" - ./%s -D dictionary-search\n", app);
    printf("\tList only dictionaries where id, long name or short name matches 'dictionary-search'\n");
    printf("\teg. ./%s -D english\n", app);
    printf(" - ./%s -s phrase\n", app);
    printf("\tList all suggestions for the phrase (Max 25 will be returned)\n");
    printf(" - ./%s -s phrase -D dictionary-id\n", app);
    printf("\tList all suggestions for the phrase but only in the dictionary with id of dictionary-id.\n");
    printf("\t * Max 25 will be returned\n");
    printf("\teg. ./%s -s 'hello' -D 2-1\n", app);
}

// print all dictionaries
void print_all_dictionaries(const char *search)
{
    // download all dictionaries 
   struct wordbook_array_dictionary *dict_array;
    dict_array = wordbook_get_dictionaries();

    // loop over all dictionaries, and print their info!
    for (size_t i = 0; i < dict_array->count; i++)
    {
        struct wordbook_dictionary dict = dict_array->dicts[i];
        bool print = true;
        if (search != NULL)
        {
#ifdef _MSC_VER
            print = _strcmpi(dict.id, search) == 0;
#else
            print = strcmpi(dict.id, search) == 0;
#endif
            if (!print)
            {
#ifdef _MSC_VER
                print = _strcmpi(dict.short_name, search) == 0;
#else
                print = strcmpi(dict.short_name, search) == 0;
#endif
                if (!print && (strstr_i(dict.short_name, search) != NULL))
                {
                    print = true;
                }
            }
            if (!print)
            {
#ifdef _MSC_VER
                print = _strcmpi(dict.long_name, search) == 0;
#else
                print = strcmpi(dict.long_name, search) == 0;
#endif
                if (!print && (strstr_i(dict.long_name, search) != NULL))
                {
                    print = true;
                }
            }
        }
        //dict.info;
        //dict.alphabet;
        //dict.url;
        if (print)
        {
            printf("id .......: %s\n", dict.id);
            printf("long name : %s\n", dict.long_name);
            printf("short name: %s\n\n", dict.short_name);
        }
    }

    // free all memory usen by the downloaded dictionaries
    wordbook_array_dictionary_free(dict_array);
}

// convert all chars to lower and reutrn strstr (Return cannot be used! Only checked!)
char *strstr_i(const char *s1, const char *s2)
{
    char *p1 = s1;
    char *p2 = s2;
    int len1 = strlen(p1);
    int len2 = strlen(p2);
    for (size_t i = 0; i < len1; i++)
    {
        p1[i] = tolower(p2[i]);
    }

    for (size_t i = 0; i < len2; i++)
    {
        p2[i] = tolower(p2[i]);
    }
    return strstr(p1, p2);
}