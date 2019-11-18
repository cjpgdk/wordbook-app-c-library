#define _CRT_SECURE_NO_WARNINGS
#define __GNU_VISIBLE 1

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "wordbooklib.h"
#include "growablestring.h"
#include "wordbook-lookup.h"

#if defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
#define strcasecmp _stricmp
#include "getopt.h"
#else
#include <strings.h>
#include <getopt.h>
#endif


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
    while ((c = getopt(argc, argv, "L:ls:h")) != -1)
    {
        switch (c)
        {
        case 'l':
            print_dicts = true;
            break;
        case 'L':
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

    // a fix, if print_suggests and print_dict
    // but using lowercase l for dictionary id!
    if (print_dicts && print_suggests)
    {
        if (print_dict == NULL)
        {
            print_dicts = false;
        }
    }

    // print dictionaries.
    // if print_suggests then we skip and use the dict_id in the suggestions lookup
    if (print_dicts && !print_suggests)
    {
        print_all_dictionaries(print_dict);
        return EXIT_SUCCESS;
    }

    // print suggextions.
    // if we have a dictionary id we use that to show the definition
    if (print_suggests)
    {
        wordbook_array_suggestions_t suggestion_array = wordbook_get_suggestions(print_suggest, print_dict);

        for (size_t i = 0; i < suggestion_array->count; i++)
        {
            struct wordbook_suggestion suggestion = suggestion_array->suggestions[i];

            printf("Word id .........: %i\n", suggestion.word_id);
            printf("word ............: %s\n", suggestion.word);
            printf("Src language name: %s\n", suggestion.source_language_name);
            printf("Src language id .: %i\n", suggestion.source_language_id);
            if (print_dicts)
            {
                wordbook_array_definition_t res = wordbook_get_definitions(
                    suggestion.word_id,
                    NULL, suggestion.source_language_id,
                    get_destination_language_id_from_dict_id(print_dict)
                );
                if (res != NULL)
                {
                    printf("------------------\n");
                    printf("-  Definitions   -\n");
                    printf("------------------\n");
                    for (size_t i = 0; i < res->count; i++)
                    {
                        struct wordbook_definition definition = res->definitions[i];

                        // build a formatted view of the dictionary!
                        growable_string_t gstr = growable_string_new(strlen(definition.dictionary) + 5);
                        growable_string_append_cstr(gstr, definition.dictionary);
                        growable_string_append_cstr(gstr, " (");
                        growable_string_append_int(gstr, definition.source_language_id);
                        growable_string_append_char(gstr, '-');
                        growable_string_append_int(gstr, definition.destination_language_id);
                        growable_string_append_char(gstr, ')');

                        printf("Dictionary: %s\n", gstr->s);
                        printf("Definition:\n%s\n", definition.definition);

                        growable_string_delete(gstr);
                    }
                }
            }
            printf("--------------------------------------------------------\n\n");
        }

        // free all memory used by the downloaded dictionaries
        wordbook_array_suggestion_free(suggestion_array);
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
    printf(" - ./%s -l\n", app);
    printf("\tList all dictionaries\n");
    printf(" - ./%s -L dictionary-search\n", app);
    printf("\tList only dictionaries where id, long name or short name matches 'dictionary-search'\n");
    printf("\teg. ./%s -L english\n", app);
    printf(" - ./%s -s phrase\n", app);
    printf("\tList all suggestions for the phrase (Max 25 will be returned)\n");
    printf(" - ./%s -s phrase -L dictionary-id\n", app);
    printf("\tList all suggestions for the phrase but only in the dictionary with id of dictionary-id.\n");
    printf("\t * Max 25 will be returned\n");
    printf("\teg. ./%s -s 'hello' -L 2-1\n", app);
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
            print = strcasecmp(dict.id, search) == 0;
            if (!print)
            {
                print = strcasecmp(dict.short_name, search) == 0;
                if (!print && (findinstr(dict.short_name, search) == 1))
                {
                    print = true;
                }
            }
            if (!print)
            {
                print = strcasecmp(dict.long_name, search) == 0;
                if (!print && (findinstr(dict.long_name, search) == 1))
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

    // free all memory used by the downloaded dictionaries
    wordbook_array_dictionary_free(dict_array);
}

int findinstr(const char *str, const char *find)
{

    char *s1 = malloc((strlen(str) + 1) * sizeof(char));
    if (s1 == NULL)
    {
        return 0;
    }
    strcpy(s1, str);
    
    char *s2 = malloc((strlen(find) + 1) * sizeof(char));
    if (s2 == NULL)
    {
        free(s1);
        return 0;
    }
    strcpy(s2, find);

    size_t len = strlen(s1);
    for (size_t i = 0; i < len; i++)
    {
        s1[i] = tolower(s1[i]);
    }

    len = strlen(s2);
    for (size_t i = 0; i < len; i++)
    {
        s2[i] = tolower(s2[i]);
    }

    int result = strstr(s1, s2) != NULL;

    free(s2);
    free(s1);
    return result;
}