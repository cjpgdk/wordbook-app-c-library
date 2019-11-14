
#define _CRT_SECURE_NO_WARNINGS
#include "growablestring.h"
#include "wordbooklib.h"
#include "curlhelper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

void wordbook_get_suggestions(const char *query, const char *dict_id)
{
    curl_download_result json_result;
    if (dict_id == NULL)
    {
        json_result = wordbook_get_suggestions_json(query);
    } 
    else
    {
        json_result = wordbook_get_dictionary_suggestions_json(query, dict_id);
    }

    printf("%s\n", json_result.ptr);
    free(json_result.ptr);
}

// get suggestions from wordbook.cjpg.app, based on the query and the selected dictionary
curl_download_result wordbook_get_dictionary_suggestions_json(const char *query, const char *dict_id)
{
    int len = strlen(API_BASE_URL API_PATH_SUGGESTIONS "?");
    len += strlen(query);
    growable_string_t gstr = growable_string_new(len);
    growable_string_append_cstr(gstr, API_BASE_URL);
    growable_string_append_cstr(gstr, API_PATH_SUGGESTIONS);
    growable_string_append_cstr(gstr, "?query=");
    growable_string_append_cstr(gstr, query);
    growable_string_append_cstr(gstr, "&language=");
    growable_string_append_cstr(gstr, dict_id);
    curl_download_result result = wordbook_perform_http_get(gstr->s);
    growable_string_delete(gstr);
    return result;
}

// get suggestions from wordbook.cjpg.app, based on the query
curl_download_result wordbook_get_suggestions_json(const char *query)
{
    int len = strlen(API_BASE_URL API_PATH_SUGGESTIONS "?");
    len += strlen(query);
    growable_string_t gstr = growable_string_new(len);
    growable_string_append_cstr(gstr, API_BASE_URL);
    growable_string_append_cstr(gstr, API_PATH_SUGGESTIONS);
    growable_string_append_cstr(gstr, "?query=");
    growable_string_append_cstr(gstr, query);
    curl_download_result result = wordbook_perform_http_get(gstr->s);
    growable_string_delete(gstr);
    return result;
}

// get all available dictionaries
wordbook_array_dictionary_t wordbook_get_dictionaries()
{
    // get the raw json data
    curl_download_result json_result;
    json_result = wordbook_get_dictionaries_json();

    // declare the array to hold the dictionaries
    wordbook_array_dictionary_t dict_array = (wordbook_array_dictionary_t)malloc(sizeof(struct wordbook_array_dictionary));

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

            // initialize the dictionary array
            initialize_array_wordbook_dictionary(dict_array, dictionaries_count);

            // loop the items in the array
            for (int i = 0; i < dictionaries_count; i++)
            {
                /*
                 * Finaly we got the actual dictionary object!
                 */
                 // get the array item at index 'i'
                json_object *obj = json_object_array_get_idx(json_obj, i);

                // create a temperary dict.
                struct wordbook_dictionary dict = (struct wordbook_dictionary){ NULL,  NULL, NULL, NULL, NULL, NULL };

                // lets just confirm this is an object
                if (json_object_is_type(obj, json_type_object))
                {
                    struct json_object_iterator it;
                    struct json_object_iterator it_end;

                    it = json_object_iter_begin(obj);
                    it_end = json_object_iter_end(obj);

                    // loop the properties in the object
                    while (!json_object_iter_equal(&it, &it_end))
                    {
                        // this must be strings so we check to ignore any other data!
                        struct json_object *obj_val = json_object_iter_peek_value(&it);
                        if (json_object_is_type(obj_val, json_type_string))
                        {
                            char *name = json_object_iter_peek_name(&it);
                            char *value = json_object_get_string(obj_val);

                            // Copy alphabet
                            if (_strcmpi(name, "alphabet") == 0)
                            {
                                dict.alphabet = malloc(strlen(value) + 1);
                                if (dict.alphabet == NULL)
                                {
                                    continue;
                                }
                                strcpy(dict.alphabet, value);
                            }
                            // Copy id
                            else if (_strcmpi(name, "id") == 0)
                            {
                                dict.id = malloc(strlen(value) + 1);
                                if (dict.id == NULL)
                                {
                                    continue;
                                }
                                strcpy(dict.id, value);
                            }
                            // Copy info
                            else if (_strcmpi(name, "info") == 0)
                            {
                                dict.info = malloc(strlen(value) + 1);
                                if (dict.info == NULL)
                                {
                                    continue;
                                }
                                strcpy(dict.info, value);
                            }
                            // Copy long name
                            else if (_strcmpi(name, "long") == 0)
                            {
                                dict.long_name = malloc(strlen(value) + 1);
                                if (dict.long_name == NULL)
                                {
                                    continue;
                                }
                                strcpy(dict.long_name, value);
                            }
                            // Copy short name
                            else if (_strcmpi(name, "short") == 0)
                            {
                                dict.short_name = malloc(strlen(value) + 1);
                                if (dict.short_name == NULL)
                                {
                                    continue;
                                }
                                strcpy(dict.short_name, value);
                            }
                            // Copy url
                            else if (_strcmpi(name, "url") == 0)
                            {
                                dict.url = malloc(strlen(value) + 1);
                                if (dict.url == NULL)
                                {
                                    continue;
                                }
                                strcpy(dict.url, value);
                            }
                        }
                        // move to next object
                        json_object_iter_next(&it);
                    }
                }
                // insert the new dict into the array.
                insert_wordbook_dictionary(dict_array, dict);
                // free memory used by temp dict
                wordbook_dictionary_free_props(dict);
            }
        }
        // free the json object
        json_object_put(json_obj);
        // free the downloaded json.
        free(json_result.ptr);
    }
    return dict_array;
}

// Insert a 'wordbook_dictionary' into an wordbook_array_dictionary
void insert_wordbook_dictionary(wordbook_array_dictionary_t dict_array_struct_ptr, struct wordbook_dictionary dict)
{
    // increment the size, if needed.
    if (dict_array_struct_ptr->count == dict_array_struct_ptr->size)
    {
        dict_array_struct_ptr->size *= 2;
        dict_array_struct_ptr->dicts = 
            (wordbook_dictionary_t)realloc(dict_array_struct_ptr->dicts, dict_array_struct_ptr->size * sizeof(struct wordbook_dictionary));
        if (dict_array_struct_ptr->dicts == NULL)
        {
            fprintf(stderr, "realloc() failed in insert_wordbook_dictionary\n");
            exit(EXIT_FAILURE);
        }
        // Initialize the last/new elements of the reallocated array
        for (unsigned int i = dict_array_struct_ptr->count; i < dict_array_struct_ptr->size; i++)
        {
            memset(&dict_array_struct_ptr->dicts[i], 0, sizeof(struct wordbook_dictionary));
        }
    }

    // simplify the code, by collecting the index.
    size_t index = dict_array_struct_ptr->count;

    // Copy the alphabet.
    size_t len = strlen(dict.alphabet) + 1;
    dict_array_struct_ptr->dicts[index].alphabet = malloc(len);
    if (dict_array_struct_ptr->dicts[index].alphabet != NULL)
    {
        strcpy(dict_array_struct_ptr->dicts[index].alphabet, dict.alphabet);
    }

    // Copy the id.
    len = strlen(dict.id) + 1;
    dict_array_struct_ptr->dicts[index].id = malloc(len);
    if (dict_array_struct_ptr->dicts[index].id != NULL)
    {
        strcpy(dict_array_struct_ptr->dicts[index].id, dict.id);
    }

    // Copy the info.
    len = strlen(dict.info) + 1;
    dict_array_struct_ptr->dicts[index].info = malloc(len);
    if (dict_array_struct_ptr->dicts[index].info != NULL)
    {
        strcpy(dict_array_struct_ptr->dicts[index].info, dict.info);
    }

    // Copy the long_name.
    len = strlen(dict.long_name) + 1;
    dict_array_struct_ptr->dicts[index].long_name = malloc(len);
    if (dict_array_struct_ptr->dicts[index].long_name != NULL)
    {
        strcpy(dict_array_struct_ptr->dicts[index].long_name, dict.long_name);
    }

    // Copy the short_name.
    len = strlen(dict.short_name) + 1;
    dict_array_struct_ptr->dicts[index].short_name = malloc(len);
    if (dict_array_struct_ptr->dicts[index].short_name != NULL)
    {
        strcpy(dict_array_struct_ptr->dicts[index].short_name, dict.short_name);
    }

    // Copy the url.
    len = strlen(dict.url) + 1;
    dict_array_struct_ptr->dicts[index].url = malloc(len);
    if (dict_array_struct_ptr->dicts[index].url != NULL)
    {
        strcpy(dict_array_struct_ptr->dicts[index].url, dict.url);
    }

    dict_array_struct_ptr->count++;
}

// initialize the array struct for 'wordbook_dictionary' eith a given size!
void initialize_array_wordbook_dictionary(wordbook_array_dictionary_t dict_array_struct_ptr, size_t initial_size)
{
    // Allocate initial space
    dict_array_struct_ptr->dicts = (wordbook_dictionary_t)malloc(initial_size * sizeof(struct wordbook_dictionary));
    dict_array_struct_ptr->count = 0;
    dict_array_struct_ptr->size = initial_size;

    // Initialize all values of the array to 0
    for (int i = 0; i < initial_size; i++)
    {
        memset(&dict_array_struct_ptr->dicts[i], 0, sizeof(struct wordbook_dictionary));
    }
}

// free memory used by the 'wordbook_array_dictionary'
void wordbook_array_dictionary_free(wordbook_array_dictionary_t dict_array)
{
    if (dict_array != NULL)
    {
        if (dict_array->dicts != NULL)
        {
            for (int i = 0; i < dict_array->count; i++)
            {
                wordbook_dictionary_free_props(dict_array->dicts[i]);
            }
            free(dict_array->dicts);
        }
        free(dict_array);
    }
}

// free memory used by props in 'wordbook_dictionary'
// make sure all props are NULL or a valid pointer.
void wordbook_dictionary_free_props(struct wordbook_dictionary dict)
{
    if (dict.alphabet)
    {
        free(dict.alphabet);
    }
    if (dict.id)
    {
        free(dict.id);
    }
    if (dict.info)
    {
        free(dict.info);
    }
    if (dict.long_name)
    {
        free(dict.long_name);
    }
    if (dict.short_name)
    {
        free(dict.short_name);
    }
    if (dict.url)
    {
        free(dict.url);
    }
}

// free memory used by 'wordbook_dictionary'
void wordbook_dictionary_free(wordbook_dictionary_t dict_struct_ptr)
{
    wordbook_dictionary_free_props(*dict_struct_ptr);
    if (dict_struct_ptr != NULL)
    {
        free(dict_struct_ptr);
    }
}

// download all available dictionaries from wordbook.cjpg.app. 
curl_download_result wordbook_get_dictionaries_json()
{
    return wordbook_perform_http_get(API_BASE_URL API_PATH_DICTIONARIES);
}