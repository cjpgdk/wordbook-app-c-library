
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
    // get the raw json data
    curl_download_result json_result;
    json_result = wordbook_get_dictionaries_json();

    wordbook_dictionary **dictionaries = NULL;
    int dictionaries_count = 0;
    
    // make sure we got a pointer
    if (json_result.ptr != NULL)
    {
        // parse the json string
        struct json_object *json_obj;
        json_obj = json_tokener_parse(json_result.ptr);
        // get the type of data, making sure it's an array
        if (json_object_is_type(json_obj, json_type_array))
        {
            // get the dictionary counter.
            dictionaries_count = json_object_array_length(json_obj);
            /*
            dictionaries = (wordbook_dictionary **)malloc(dictionaries_count * sizeof(wordbook_dictionary));
            */
            // loop the items in the array
            for (int i = 0; i < dictionaries_count; i++)
            {
                /*
                 * Finaly we got the actual dictionary object!
                 */
                // get the array item at index 'i'
                json_object* obj = json_object_array_get_idx(json_obj, i);
                // kets just confirm this is an object
                printf("Index: %d\n", i);
                printf("=======\n");
                if (json_object_is_type(obj, json_type_object))
                {
                    struct json_object_iterator it;
                    struct json_object_iterator it_end;
                    it = json_object_iter_begin(obj);
                    it_end = json_object_iter_end(obj);
                    while (!json_object_iter_equal(&it, &it_end)) {
                        
                        // this must be strings so we checj to ignore any other data!
                        struct json_object* obj_val = json_object_iter_peek_value(&it);
                        if (json_object_is_type(obj_val, json_type_string))
                        {
                            char *name = json_object_iter_peek_name(&it);
                            char *value = json_object_get_string(obj_val);
                            /*
                            if (name == "alphabet")
                            {
                                dictionaries[i]->alphabet = malloc(strlen(value) + 1);
#ifdef _MSC_VER
                                strcpy_s(dictionaries[i]->alphabet, strlen(value) + 1, value);
#else
                                strcpy(dictionaries[i]->alphabet, value);
#endif

                            }
                            */
                            printf("\t%s\n", name);
                            printf("\t%s\n", value);
                        }
                        json_object_iter_next(&it);
                    }
                }
                printf("=======\n");
                //printf("[%d]=%s\n", (int)i, json_object_to_json_string(obj));
            }
        }
        //printf("Result after parse:\n%s\n\n", json_object_to_json_string(json_obj));
        json_object_put(json_obj);
        free(json_result.ptr);
    }
    /*
    if (dictionaries != NULL)
    {
        for (int i = 0; i < dictionaries_count; i++)
        {
            free(dictionaries[i]->alphabet);
        }
        free(dictionaries);
    }
    */
    return 0;
}